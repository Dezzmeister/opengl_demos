#pragma once
#include <memory>
#include <vector>
#include "bounding_volumes.h"

namespace phys {
	template <bounding_volume Volume, typename Identifier>
	class bvh {
	public:
		void insert(Identifier id, const Volume &vol);
		bool remove(Identifier id);
		void update(Identifier id, const Volume &vol);
		bool has(Identifier id) const;
		size_t size() const;

	private:
#ifdef DEBUG
	public:
#endif
		struct node {
			Volume vol{};
			Identifier id{};

			node * parent{};
			std::unique_ptr<node> left{};
			std::unique_ptr<node> right{};

			node(const Volume &_vol, Identifier _id);

			bool is_leaf() const;
			void recalculate_parent_volumes();
		};

		struct id_node {
			Identifier id{};
			node * n{};

			id_node(Identifier _id);
			id_node(Identifier _id, node * _n);

			friend bool operator<(const id_node &a, const id_node &b) {
				return a.id < b.id;
			}

			friend bool operator==(const id_node &a, const id_node &b) {
				return a.id == b.id;
			}
		};

		std::vector<id_node> ids{};
		std::unique_ptr<node> root{};
	};
}

template <phys::bounding_volume Volume, typename Identifier>
phys::bvh<Volume, Identifier>::node::node(const Volume &_vol, Identifier _id) :
	vol(_vol), id(_id) {}

template <phys::bounding_volume Volume, typename Identifier>
bool phys::bvh<Volume, Identifier>::node::is_leaf() const {
	return (! left.get()) && (! right.get());
}

template <phys::bounding_volume Volume, typename Identifier>
void phys::bvh<Volume, Identifier>::node::recalculate_parent_volumes() {
	assert(!! left.get() == !! right.get());

	if (left.get()) {
		vol = Volume(left->vol, right->vol);
	}

	if (parent) {
		parent->recalculate_parent_volumes();
	}
}

template <phys::bounding_volume Volume, typename Identifier>
phys::bvh<Volume, Identifier>::id_node::id_node(Identifier _id) :
	id(_id), n(nullptr) {}

template <phys::bounding_volume Volume, typename Identifier>
phys::bvh<Volume, Identifier>::id_node::id_node(Identifier _id, phys::bvh<Volume, Identifier>::node * _n) :
	id(_id), n(_n) {}

template <phys::bounding_volume Volume, typename Identifier>
void phys::bvh<Volume, Identifier>::insert(Identifier id, const Volume &vol) {
	auto id_i = std::lower_bound(std::begin(ids), std::end(ids), id);

	if (id_i != std::end(ids) && id_i->id == id) {
		return;
	}

	if (! root.get()) {
		root = std::make_unique<node>(vol, id);
		ids.insert(id_i, id_node(id, root.get()));
		return;
	}

	node * n = root.get();

	while (! n->is_leaf()) {
		real left_growth = n->left->vol.growth(vol);
		real right_growth = n->right->vol.growth(vol);

		if (left_growth < right_growth) {
			n = n->left.get();
		} else {
			n = n->right.get();
		}
	}

	Identifier old_id = n->id;

	n->left = std::make_unique<node>(n->vol, old_id);
	n->right = std::make_unique<node>(vol, id);
	n->id = 0;
	n->left->parent = n;
	n->right->parent = n;

	ids.insert(id_i, id_node(id, n->right.get()));

	id_i = std::lower_bound(std::begin(ids), std::end(ids), old_id);
	assert(id_i != std::end(ids));
	assert(id_i->id == old_id);
	id_i->n = n->left.get();

	n->recalculate_parent_volumes();
}

template <phys::bounding_volume Volume, typename Identifier>
bool phys::bvh<Volume, Identifier>::remove(Identifier id) {
	auto id_i = std::lower_bound(std::begin(ids), std::end(ids), id);

	if (id_i == std::end(ids) || id_i->id != id) {
		return false;
	}

	node * n = id_i->n;

	ids.erase(id_i);

	if (n == root.get()) {
		assert(! n->parent);
		root.reset();
		return true;
	}

	assert(n->parent);
	assert(!! n->parent->left.get() == !! n->parent->right.get());

	node * parent = n->parent;

	if (n == parent->left.get()) {
		if (parent == root.get()) {
			root = std::move(parent->right);
			root->parent = nullptr;
		} else {
			assert(parent->parent);

			node * grandparent = parent->parent;

			if (parent == grandparent->left.get()) {
				grandparent->left = std::move(parent->right);
				grandparent->left->parent = grandparent;
				grandparent->left->recalculate_parent_volumes();
			} else {
				grandparent->right = std::move(parent->right);
				grandparent->right->parent = grandparent;
				grandparent->right->recalculate_parent_volumes();
			}
		}
	} else {
		assert(n == parent->right.get());

		if (parent == root.get()) {
			root = std::move(parent->left);
			root->parent = nullptr;
		} else {
			assert(parent->parent);

			node * grandparent = parent->parent;

			if (parent == grandparent->left.get()) {
				grandparent->left = std::move(parent->left);
				grandparent->left->parent = grandparent;
				grandparent->left->recalculate_parent_volumes();
			} else {
				grandparent->right = std::move(parent->left);
				grandparent->right->parent = grandparent;
				grandparent->right->recalculate_parent_volumes();
			}
		}
	}

	return true;
}

template <phys::bounding_volume Volume, typename Identifier>
void phys::bvh<Volume, Identifier>::update(Identifier id, const Volume &vol) {
	bool was_present = remove(id);

	if (was_present) {
		insert(id, vol);
	}
}

template <phys::bounding_volume Volume, typename Identifier>
bool phys::bvh<Volume, Identifier>::has(Identifier id) const {
	id_node n(id, 0);

	auto id_i = std::lower_bound(std::begin(ids), std::end(ids), n);

	return (id_i != std::end(ids)) && id_i->id == id;
}

template <phys::bounding_volume Volume, typename Identifier>
size_t phys::bvh<Volume, Identifier>::size() const {
	return ids.size();
}
