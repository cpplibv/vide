/*
  Copyright (c) 2017, Randolph Voorhies, Shane Grant
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      * Neither the name of the copyright holder nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <vide/types/memory.hpp>
#include <vide/types/std_string.hpp>
#include <vide/types/std_vector.hpp>

#include <algorithm>
#include <cstddef>
#include <memory>
#include <ostream>
#include <random>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include <doctest/doctest.h>

#include "common/archives.hpp"
#include "common/check_roundtrip.hpp"
#include "common/random.hpp"
#include "common/sample_structs.hpp"


struct DeferNode;

struct DeferRelation {
	std::shared_ptr<DeferNode> node;
	int x = 0;
	std::string y;

	template <class Archive>
	void serialize(Archive& ar) {
		ar(node);
		ar(x);
		ar(y);
	}

	bool operator==(const DeferRelation& other) const;
};

/// Every member but the plain ones is deferred; relations point at nodes anywhere in the graph
struct DeferNode {
	std::size_t id = 0;
	long w = 0;
	StructInternalSerialize iser;
	StructInternalSplit ispl;
	StructExternalSerialize eser;
	StructExternalSplit espl;
	std::vector<DeferRelation> relations;
	char z = 0;

	DeferNode() = default;
	DeferNode(std::size_t id, std::mt19937& gen) :
		id(id),
		w(random_value<long>(gen)),
		iser(random_value<StructInternalSerialize>(gen)),
		ispl(random_value<StructInternalSplit>(gen)),
		eser(random_value<StructExternalSerialize>(gen)),
		espl(random_value<StructExternalSplit>(gen)),
		z(random_value<char>(gen)) {}

	template <class Archive>
	void serialize(Archive& ar) {
		ar(id);
		ar(w);
		ar.defer(iser);
		ar.defer(ispl);
		ar.defer(eser);
		ar.defer(espl);
		ar.defer(relations);
		ar(z);
	}

	bool operator==(const DeferNode&) const = default;

	friend std::ostream& operator<<(std::ostream& os, const DeferNode& node) {
		return os << "[id: " << node.id << " relations: " << node.relations.size() << "]";
	}
};

/// Relations compare their target by id (0 for null, real ids start at 1); comparing the nodes would recurse through
/// cycles
bool DeferRelation::operator==(const DeferRelation& other) const {
	const auto id = [](const std::shared_ptr<DeferNode>& target) { return target ? target->id : 0; };
	return x == other.x && y == other.y && id(node) == id(other.node);
}

using Nodes = std::vector<std::shared_ptr<DeferNode>>;

constexpr std::size_t nodes_per_list = 10;
constexpr std::size_t max_relations = 20;

/// Two node lists; the relations of both point into the first
std::pair<Nodes, Nodes> random_graph(std::mt19937& gen) {
	std::size_t id = 1;
	Nodes first;
	Nodes second;
	for (std::size_t i = 0; i < nodes_per_list; ++i)
		first.push_back(std::make_shared<DeferNode>(id++, gen));
	for (std::size_t i = 0; i < nodes_per_list; ++i)
		second.push_back(std::make_shared<DeferNode>(id++, gen));

	for (const Nodes* nodes : {&first, &second})
		for (const auto& node : *nodes) {
			node->relations.resize(random_index(0, max_relations, gen));
			for (auto& relation : node->relations)
				relation = {first[random_index(0, first.size() - 1, gen)], random_value<int>(gen), random_value<std::string>(gen)};
		}
	return {std::move(first), std::move(second)};
}

/// The pointees are equal; pointer identity cannot match across a round trip
bool same_nodes(const Nodes& a, const Nodes& b) {
	return std::ranges::equal(a, b, [](const auto& x, const auto& y) { return x && y && *x == *y; });
}

TEST_CASE_TEMPLATE("defer", Ar, VIDE_ALL_ARCHIVES) {
	auto gen = test_rng();
	const auto [first, second] = random_graph(gen);

	const CallLog calls;

	std::ostringstream os;
	{
		typename Ar::output ar(os);
		ar(first);
		ar(second);
		ar.serializeDeferments();
	}

	Nodes loaded_first;
	Nodes loaded_second;
	std::istringstream is(os.str());
	{
		typename Ar::input ar(is);
		ar(loaded_first);
		ar(loaded_second);
		ar.serializeDeferments();
	}

	CHECK(same_nodes(loaded_first, first));
	CHECK(same_nodes(loaded_second, second));

	// Nodes reached again through relations are references, so each struct still runs once per node
	const std::size_t node_count = first.size() + second.size();
	calls.check<StructInternalSerialize>(node_count);
	calls.check<StructInternalSplit>(node_count);
	calls.check<StructExternalSerialize>(node_count);
	calls.check<StructExternalSplit>(node_count);
}
