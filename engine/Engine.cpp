#include "Engine.hpp"

#include "render/UI/UI.hpp"

namespace engn {
	Engine::Engine() :
		m_camera{ new rend::EngineCamera{60.0f, WIN_WIDTH_DEF, WIN_HEIGHT_DEF, {0.0f, 0.0f, -2.0f}} },
		m_window{ new win::Window<WIN_WIDTH_DEF, WIN_HEIGHT_DEF>() }
	{
		m_graphics.init();
		// ImGui
		rend::UI::instance().init(m_window->getHandler());
	}

	void Engine::setWindowSize(int screenWidth, int screenHeight) {
		m_camera->setNewScreenSize(screenWidth, screenHeight);
	}

	void Engine::setEngineData(const rend::RenderData& data)
	{
		m_renderData.iTime = data.iTime;
		m_renderData.iDt = data.iDt;
		m_renderData.iResolutionX = m_window->getWidth();
		m_renderData.iResolutionY = m_window->getHeight();
		m_renderData.invResolutionX = 1.0f / m_window->getWidth();
		m_renderData.invResolutionY = 1.0f / m_window->getHeight();
	}

	void Engine::initScene()
	{
		rend::LightSystem::getInstance().addDirLight(
			{ 0.0f, -0.8f, 0.6f }, light::WHITE, 0.5f
		);
		rend::LightSystem::getInstance().addPointLight(
			XMMatrixTranslation(5.0f, 5.0f, 7.0f), light::WHITE, 2.0f
		);
		rend::LightSystem::getInstance().addPointLight(
			XMMatrixTranslation(-3.0f, 7.0f, 9.0f), { 3.0f, 0.39f, 0.39f }, 0.8f
		);
		rend::LightSystem::getInstance().addPointLight(
			XMMatrixTranslation(0.0f, 0.0f, 8.0f), light::LIGHTGREEN, 0.8f
		);

		rend::LightSystem::getInstance().setSpotLightSettings(
			17.0f, light::WHITE, 2.0f
		);

		std::shared_ptr<mdl::Model> mptr = mdl::ModelManager::getInstance().getModel(MODELS["TOWER"]);
		rend::MeshSystem::getInstance().addNormalInstance(
			mptr,
			{ },
			{ XMMatrixTranslation(0.0f, 0.0f, 10.0f), {},  {1.0f, 0.0f, 0.0f, 1.0f} }
		);

		mptr.reset();
		mptr = mdl::ModelManager::getInstance().getModel(MODELS["SAMURAI"]);
		rend::MeshSystem::getInstance().addNormalInstance(
			mptr,
			{ },
			{ XMMatrixTranslation(5.0f, 0.0f, 10.0f), {}, {1.0f, 0.0f, 0.0f, 1.0f} }
		);

		mptr.reset();
		mptr = mdl::ModelManager::getInstance().getModel(MODELS["HORSE"]);
		rend::MeshSystem::getInstance().addNormalInstance(
			mptr,
			{ },
			{ XMMatrixTranslation(-5.0f, 0.0f, 10.0f), {}, {1.0f, 0.0f, 0.0f, 1.0f} }
		);

		mptr.reset();
		mptr = mdl::ModelManager::getInstance().getModel(MODELS["HORSE"]);
		rend::MeshSystem::getInstance().addNormalInstance(
			mptr,
			{ },
			{ XMMatrixTranslation(-3.0f, 0.0f, 10.0f), {}, {1.0f, 0.0f, 0.0f, 1.0f} }
		);

		mptr.reset();
		mptr = mdl::ModelManager::getInstance().getModel(MODELS["CUBE"]);
		rend::MeshSystem::getInstance().addNormalInstance(
			mptr,
			{
				tex::TextureManager::getInstance().getTexture(MATERIALS["COBBLESTONE"].albedo),
				tex::TextureManager::getInstance().getTexture(MATERIALS["COBBLESTONE"].normalMap),
				tex::TextureManager::getInstance().getTexture(MATERIALS["COBBLESTONE"].roughness),
				tex::TextureManager::getInstance().getTexture(MATERIALS["COBBLESTONE"].metallic),
			},
			{ XMMatrixTranslation(7.0f, 0.0f, 10.0f), {}, {1.0f, 0.0f, 0.0f, 1.0f} }
		);

		// A cube that you can play with via editing the reflection
		mptr.reset();
		mptr = mdl::ModelManager::getInstance().getModel(MODELS["CUBE"]);
		rend::MeshSystem::getInstance().addNormalInstance(
			mptr,
			{
				tex::TextureManager::getInstance().getTexture(MATERIALS["TEST"].albedo),
			},
			{ XMMatrixTranslation(-7.0f, 0.0f, 10.0f), {}, {1.0f, 0.0f, 0.0f, 1.0f} }
		);


		// Fill the field with cubes
		for (int i = -24; i < 24; ++i) {
			for (int j = -24; j < 32; ++j) {
				mptr.reset();
				mptr = mdl::ModelManager::getInstance().getModel(MODELS["CUBE"]);
				rend::MeshSystem::getInstance().addNormalInstance(
					mptr,
					{
						tex::TextureManager::getInstance().getTexture(MATERIALS["COBBLESTONE"].albedo),
						tex::TextureManager::getInstance().getTexture(MATERIALS["COBBLESTONE"].normalMap),
						tex::TextureManager::getInstance().getTexture(MATERIALS["COBBLESTONE"].roughness)
					},
					{ XMMatrixScaling(1.0f, 0.3f, 1.0f) * XMMatrixTranslation(2 * i, -2.0f, 2 * j), {}, {1.0f, 0.0f, 0.0f, 1.0f} }
				);
			}
		}
	}

	bool Engine::run()
	{
		rend::UI::instance().startFrame();
		handlePhysics();
		return render();
	}

	bool Engine::render() {
		if (m_window->pollResize()) {
			setWindowSize(m_window->getWidth(), m_window->getHeight());
		}
		if (!m_graphics.renderFrame(m_camera, m_window, m_renderData, m_renderFlags)) {
			return false;
		}
		m_window->present();
		return true;
	}

	void Engine::handlePhysics() {
		handleCameraRotation();
		handleCameraMovement();

		handleDragging();

		handleSpawning();
		handleParticleSpawning();

		fillRenderModeFlagsFromInput();
	}

	void Engine::handleSpawning()
	{
		auto& keyboard = inp::Keyboard::getInstance();
		if (!ImGui::IsWindowFocused() && keyboard.isKeyJustPressed(inp::Keyboard::Keys::KEY_N)) {
			m_spawner.addDissolutionInstance(m_camera, m_renderData.iTime);
		}

		m_spawner.updateInstances(m_renderData.iTime);
	}

	void Engine::handleParticleSpawning()
	{
		auto& keyboard = inp::Keyboard::getInstance();
		if (!ImGui::IsWindowFocused() && keyboard.isKeyJustPressed(inp::Keyboard::Keys::KEY_H)) {
			rend::ParticleSystem::getInstance().addSmokeEmitter(
				m_camera->getCamPosition() + 5.0f * m_camera->getCamForward(),
				XMVECTOR{1.0f, 0.0f, 0.0f, 1.0f},
				"",
				1.0f,
				200.0f,
				2.0f
			);
		}
	}

	void Engine::handleCameraMovement() {
		bool cameraMoved = false;

		XMVECTOR position{ 0.0f, 0.0f, 0.0f, 0.0f };
		for (const auto& key : rend::EngineCamera::CameraSettings::MOVE_KEYS) {
			if (inp::Keyboard::getInstance().isKeyPressed(key)) {
				if (!cameraMoved) { cameraMoved = true; }
				position += rend::EngineCamera::CameraSettings::MOVE_TO_ACTION[key];
			}
		}
		if (cameraMoved) {
			position = XMVector3Normalize(position);
			m_camera->addRelativeOffset(position * rend::EngineCamera::CameraSettings::CAMERA_SPEED * m_renderData.iDt);
		}
	}

	void Engine::handleCameraRotation() {
		bool cameraRotated = false;
		XMVECTOR rotation{ 0.0f, 0.0f, 0.0f };

		// pitch and yaw
		auto& mouse = inp::Mouse::getInstance();
		XMVECTOR& offset = mouse.getMoveData().mouseOffset;
		if (!ImGui::IsWindowFocused() && mouse.isLMBPressed() && XMVectorGetX(XMVector2LengthSq(offset)) > 0.0f) {
			if (!cameraRotated) { cameraRotated = true; }
			rotation = XMVectorSetX(rotation, XMVectorGetY(offset));
			rotation = XMVectorSetY(rotation, XMVectorGetX(offset));
			mouse.getMoveData().mouseOffset = XMVECTOR{ 0, 0 };
		}

		if (cameraRotated) {
			rotation = XMVector3Normalize(rotation);
			m_camera->addWorldRotationMat(rotation * rend::EngineCamera::CameraSettings::ROTATION_SPEED * m_renderData.iDt);
		}
	}

	void Engine::handleDragging() {
		findDraggable();
		moveDraggable();
	}

	void Engine::findDraggable() {
		auto& mouse = inp::Mouse::getInstance();

		if (!ImGui::IsWindowFocused() && mouse.isRMBPressed() && !m_dragger.isMeshCaptured()) {
			m_dragger.capture(m_camera);
		}
		if (!ImGui::IsWindowFocused() && !mouse.isRMBPressed()) {
			m_dragger.release();
		}
	}

	void Engine::moveDraggable() {
		if (m_dragger.isMeshCaptured()) {
			m_dragger.drag(m_camera);
		}
	}
	void Engine::fillRenderModeFlagsFromInput()
	{
		if (inp::Keyboard::getInstance().isKeyJustPressed(inp::Keyboard::Keys::KEY_F)) {
			m_renderFlags.bindFlashlight = !m_renderFlags.bindFlashlight;
		}
	}
} // engn