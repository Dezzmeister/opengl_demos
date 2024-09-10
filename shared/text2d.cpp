#include "shader_store.h"
#include "text2d.h"
#include "texture_store.h"
#include "util.h"

namespace {
	constexpr util::str_kv_pair<int> text_shader_locs[] = {
		{ "x_offset", 1 },
		{ "y_offset", 2 },
		{ "glyph_width", 3 },
		{ "glyph_height", 4 },
		{ "screen_width", 5 },
		{ "screen_height", 6 },
		{ "font", 7 },
		{ "bg_color", 8 },
		{ "fg_color", 9 }
	};
}

font::font(texture _font_bmp, int _glyph_width, int _glyph_height) :
	font_bmp(std::move(_font_bmp)),
	glyph_width(_glyph_width),
	glyph_height(_glyph_height)
{}

text2d_renderer::text2d_renderer(event_buses &_buses) :
	event_listener<program_start_event>(&_buses.lifecycle, -99),
	event_listener<screen_resize_event>(&_buses.render),
	buses(_buses),
	text_vao(0, [](unsigned int vao) {
		glDeleteVertexArrays(1, &vao);
	}),
	text_vbo(0, [](unsigned int vbo) {
		glDeleteBuffers(1, &vbo);
	})
{
	event_listener<program_start_event>::subscribe();
	event_listener<screen_resize_event>::subscribe();

	glGenVertexArrays(1, &text_vao);
	glBindVertexArray(text_vao);
	glGenBuffers(1, &text_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, text_vbo);
	glVertexAttribIPointer(0, 1, GL_UNSIGNED_BYTE, sizeof(char), 0);
	glEnableVertexAttribArray(0);
	glBufferData(GL_ARRAY_BUFFER, sizeof tmp_buf, NULL, GL_STREAM_DRAW);
}

const font& text2d_renderer::load_font(const std::string &font_name, const std::string &font_path, int glyph_width, int glyph_height) {
	fonts.insert(std::make_pair(font_name, font(texture(font_path.c_str(), false), glyph_width, glyph_height)));

	const font &f = fonts.at(font_name);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, f.font_bmp.get_id());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	return fonts.at(font_name);
}

const font& text2d_renderer::get_font(const std::string &font_name) const {
	return fonts.at(font_name);
}

void text2d_renderer::draw_text(
	const std::string &text,
	const font &f,
	int x,
	int y,
	int max_width,
	int max_height,
	int line_spacing,
	const glm::vec4 &fg_color,
	const glm::vec4 &bg_color
) const {
	static constexpr int glyph_width_loc = util::find_in_map(text_shader_locs, "glyph_width");
	static constexpr int glyph_height_loc = util::find_in_map(text_shader_locs, "glyph_height");
	static constexpr int screen_width_loc = util::find_in_map(text_shader_locs, "screen_width");
	static constexpr int screen_height_loc = util::find_in_map(text_shader_locs, "screen_height");
	static constexpr int font_loc = util::find_in_map(text_shader_locs, "font");
	static constexpr int fg_color_loc = util::find_in_map(text_shader_locs, "fg_color");
	static constexpr int bg_color_loc = util::find_in_map(text_shader_locs, "bg_color");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, f.font_bmp.get_id());

	tex_shader->use();
	tex_shader->set_uniform(glyph_width_loc, f.glyph_width);
	tex_shader->set_uniform(glyph_height_loc, f.glyph_height);
	tex_shader->set_uniform(screen_width_loc, screen_width);
	tex_shader->set_uniform(screen_height_loc, screen_height);
	tex_shader->set_uniform(font_loc, 0);
	tex_shader->set_uniform(fg_color_loc, fg_color);
	tex_shader->set_uniform(bg_color_loc, bg_color);

	shader_use_event shader_event(*tex_shader);
	buses.render.fire(shader_event);

	glBindVertexArray(text_vao);
	glBindBuffer(GL_ARRAY_BUFFER, text_vbo);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	size_t in_char_pos = 0;
	size_t out_char_pos = 0;
	int draw_x = x;
	int curr_x = x;
	int curr_y = y;

	while (in_char_pos < text.size()) {
		bool text_too_tall = max_height && ((curr_y - y) > max_height);
		bool text_too_long = max_width && (curr_x - x + f.glyph_width) > max_width;
		bool text_buf_full = out_char_pos == util::c_arr_size(tmp_buf);

		if (text_too_tall) {
			break;
		}

		if (text_too_long) {
			if (! out_char_pos) {
				break;
			}

			draw_line(out_char_pos, draw_x, curr_y);
			draw_x = x;
			curr_x = x;
			curr_y += f.glyph_height + line_spacing;
			out_char_pos = 0;
			continue;
		}

		if (text_buf_full) {
			draw_line(out_char_pos, draw_x, curr_y);
			curr_x += (int)(out_char_pos * f.glyph_width);
			draw_x = curr_x;
			out_char_pos = 0;
			continue;
		}

		const char c = text.at(in_char_pos);

		if (c == '\n') {
			draw_line(out_char_pos, draw_x, curr_y);
			draw_x = x;
			curr_x = x;
			curr_y += f.glyph_height + line_spacing;
			out_char_pos = 0;
			in_char_pos++;
		} else if (c < ' ' || c > '~') {
			in_char_pos++;
		} else {
			tmp_buf[out_char_pos++] = c;
			curr_x += f.glyph_width;
			in_char_pos++;
		}
	}

	if (in_char_pos == text.size() && out_char_pos) {
		draw_line(out_char_pos, draw_x, curr_y);
	}

	glDisable(GL_BLEND);
}

void text2d_renderer::draw_line(size_t num_chars, int x, int y) const {
	static constexpr int x_offset_loc = util::find_in_map(text_shader_locs, "x_offset");
	static constexpr int y_offset_loc = util::find_in_map(text_shader_locs, "y_offset");

	glBufferSubData(GL_ARRAY_BUFFER, 0, num_chars * sizeof(char), tmp_buf);
	tex_shader->set_uniform(x_offset_loc, x);
	tex_shader->set_uniform(y_offset_loc, y);

	glDrawArrays(GL_POINTS, 0, (GLsizei)num_chars);
}

int text2d_renderer::handle(program_start_event &event) {
	event.text2d = this;

	tex_shader = &event.shaders->shaders.at("text2d");

	screen_width = event.screen_width;
	screen_height = event.screen_height;

	load_font("spleen_6x12", "../resources/spleen_font_6x12.png", 6, 12);

	return 0;
}

int text2d_renderer::handle(screen_resize_event &event) {
	screen_width = event.new_width;
	screen_height = event.new_height;

	return 0;
}