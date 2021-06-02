#pragma once

#include "../ecs.h"
#include "core/string/string_name.h"
#include "core/templates/local_vector.h"
#include "core/templates/vector.h"

class Pipeline;

class ExecutionGraph {
	friend class PipelineBuilder;

public:
	struct SystemNode {
		bool is_used = false;
		godex::system_id id = godex::SYSTEM_NONE;
		Phase phase = PHASE_PROCESS;
		// Set by the bundle. When -1 the implicit priority is used instead.
		// Has more importance than the implicit priority.
		int explicit_priority = -1;
		StringName bundle_name;

		// The list of dependencies.
		// This has more importance than the explicit priority.
		LocalVector<SystemNode *> execute_after;

		// List element pointer used to access fast.
		List<SystemNode *>::Element *self_list_element = nullptr;
	};

	struct StageNode {
		LocalVector<SystemNode *> systems;
	};

private:
	/// List of all the application systems.
	LocalVector<SystemNode> systems;
	/// List of used systems sorted by implicit and explicit priority.
	List<SystemNode *> sorted_systems;
};

class PipelineBuilder {
public:
	/// This method is used to build the pipeline `ExecutionGraph`.
	/// The `ExecutionGraph` is useful to visualize the pipeline composition.
	/// The `Pipeline` is constructed from the `ExecutionGraph`.
	static void build_graph(
			const Vector<StringName> &p_system_bundles,
			const Vector<StringName> &p_systems,
			ExecutionGraph *r_graph);

	/// This method is used to build the `Pipeline`. This method constructs the
	/// `ExecutionGraph` then it cooks it and builds the `Pipeline` from it.
	static void build_pipeline(
			const Vector<StringName> &p_system_bundles,
			const Vector<StringName> &p_systems,
			Pipeline *r_pipeline);

	/// This is method accepts the `ExecutionGraph` and build the pipeline from it.
	static void build_pipeline(
			const ExecutionGraph &p_graph,
			Pipeline *r_pipeline);

private:
	static void fetch_bundle_info(
			const Vector<StringName> &p_system_bundles,
			ExecutionGraph *r_graph);

	static void fetch_system_info(
			const StringName &p_system,
			const StringName &p_bundle_name,
			int p_explicit_priority,
			const LocalVector<Dependency> &p_extra_dependencies,
			ExecutionGraph *r_graph);

	static void resolve_dependencies(
			godex::system_id id,
			const LocalVector<Dependency> &p_dependencies,
			ExecutionGraph *r_graph);

	static void system_sorting(ExecutionGraph *r_graph);
};
