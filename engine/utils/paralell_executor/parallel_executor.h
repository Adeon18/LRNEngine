#pragma once

#include <vector>
#include <atomic>
#include <thread>
#include <functional>
#include <shared_mutex>


#define BREAK __debugbreak();

#define ALWAYS_ASSERT(expression, ...) \
	if (!(expression)) \
	{ \
		BREAK; \
		std::abort(); \
	}

#ifdef NDEBUG
#define DEV_ASSERT(...)
#else
#define DEV_ASSERT(expression, ...) ALWAYS_ASSERT(expression, __VA_ARGS__);
#endif


namespace engn {
	namespace util {
		class ParallelExecutor
		{
		public:
			static const uint32_t MAX_THREADS;
			static const uint32_t HALF_THREADS;
			using Func = std::function<void(uint32_t, uint32_t)>; // (threadIndex, taskIndex)

			ParallelExecutor(uint32_t numThreads);
			~ParallelExecutor();

			//! Getters
			uint32_t numThreads() const { return static_cast<uint32_t>(m_threads.size()); }
			bool isWorking() const { return m_finishedThreadNum < m_threads.size(); }

			//! Wait for the waitCV to unblock
			void wait();

			//! Executes a function in parallel blocking the caller thread.
			void execute(const Func& func, uint32_t numTasks, uint32_t tasksPerBatch);

			//! Executes a function in parallel asynchronously.
			void executeAsync(const Func& func, uint32_t numTasks, uint32_t tasksPerBatch);

		protected:
			void workLoop(uint32_t threadIndex);
			void awake() { m_workCV.notify_all(); }

			bool m_isLooping;

			std::atomic<uint32_t> m_finishedThreadNum;
			std::atomic<uint32_t> m_completedBatchNum;
			std::function<void(uint32_t)> m_executeTasks;

			std::shared_mutex m_mutex;
			std::condition_variable_any m_waitCV;
			std::condition_variable_any m_workCV;

			std::vector<std::thread> m_threads;
		};
	} // util
} // engn