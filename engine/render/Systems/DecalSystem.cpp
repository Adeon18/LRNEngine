#include "DecalSystem.hpp"

namespace engn {
	namespace rend {
		void DecalSystem::init()
		{
			initPipelines();
			initBuffers();
			initTextures();
		}
		void DecalSystem::addDecal(const InstanceProperties& insProps, const geom::MeshIntersection& insHit, std::unique_ptr<EngineCamera>& camPtr)
		{
			uint32_t matIdx = MeshSystem::getInstance().getGroupMatrixIdx(insProps);
			XMMATRIX& modelToWorld = TransformSystem::getInstance().getMatrixById(matIdx);
			XMMATRIX worldToModel = XMMatrixInverse(nullptr, modelToWorld);

			XMVECTOR offset = XMVector3Transform(insHit.pos, worldToModel);
			//std::cout << "Offset: " << offset << std::endl;

			XMVECTOR zWorld = XMVector3Normalize(camPtr->getCamPosition() - insHit.pos);
			zWorld = XMVectorSetW(zWorld, 0);

			//std::cout << "WorldBasis Z: " << zWorld << std::endl;

			XMMATRIX worldDecalBasis;
			worldDecalBasis.r[0] = camPtr->getCamRight();
			worldDecalBasis.r[1] = camPtr->getCamUp();
			worldDecalBasis.r[2] = zWorld;
			worldDecalBasis.r[3] = XMVECTOR{0, 0, 0, 1};

			auto& decalData = m_decals.emplace_back();
			decalData.decalToModel = worldDecalBasis * worldToModel;
			for (uint16_t i = 0; i < 3; ++i) {
				decalData.decalToModel.r[i] = XMVector3Normalize(decalData.decalToModel.r[i]);
			}
			decalData.decalToModel.r[3] = offset;
			decalData.modelToDecal = XMMatrixInverse(nullptr, decalData.decalToModel);
			decalData.modelInstanceID = matIdx;

			//std::cout << "Decal To Model: \n" << decalData.decalToModel << std::endl;
		}
		void DecalSystem::handleDecals()
		{
		}
		void DecalSystem::initBuffers()
		{
		}
		void DecalSystem::initPipelines()
		{
		}
		void DecalSystem::initTextures()
		{
		}
		void DecalSystem::fillInstanceBuffers()
		{
		}
		void DecalSystem::renderInternal()
		{
		}
	} // rend
} // engn