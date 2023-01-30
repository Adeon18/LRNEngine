#include "MeshSystem.hpp"

namespace engn {
	namespace rend {

		void MeshSystem::render()
		{
			m_normalGroup.render();
		}
		void NormalGroup::addModel(std::shared_ptr<mdl::Model> mod)
		{
			PerModel newModel;
			newModel.model = mod;

			PerMesh perMesh;

			PerMaterial perMat;
			perMat.material = {};
			for (auto& mesh : newModel.model->getMeshes()) {
				Instance i;
				i.modelToWorld = mesh.instances[0];
				i.color = { 1.0f, 0.0f, 0.0f, 1.0f };
				perMat.instances.push_back(i);
			}

			perMesh.push_back(perMat);

			newModel.perMesh.push_back(perMesh);
		}
		void NormalGroup::fillInstanceBuffer()
		{

		}
		void NormalGroup::render()
		{

		}
	} // rend
} // engn