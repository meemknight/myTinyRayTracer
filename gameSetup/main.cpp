#include <Windows.h>
#include "gameStructs.h"
#include <GL/glew.h>
#include <sstream>
#include "Geometry.h"

BOOL WINAPI DllMain(
	HINSTANCE hinstDLL,
	DWORD     fdwReason,
	LPVOID    lpvReserved
)
{

	if(fdwReason == DLL_PROCESS_ATTACH)
	{
	//	OutputDebugString("gameLayer: dll attached");
	}

	return true;
}

FreeListAllocator* allocator = nullptr;
Console* console = nullptr;

#pragma region allocator

void* operator new  (std::size_t count)
{
	auto a = allocator->threadSafeAllocate(count);


	return a;
}

void* operator new[](std::size_t count)
{
	auto a = allocator->threadSafeAllocate(count);

	return a;
}

void operator delete  (void* ptr)
{

	allocator->threadSafeFree(ptr);
}

void operator delete[](void* ptr)
{

	allocator->threadSafeFree(ptr);
}

#pragma endregion

extern "C" __declspec(dllexport) void onCreate(GameMemory * mem, HeapMemory * heapMemory,
	WindowSettings * windowSettings, PlatformFunctions * platformFunctions)
{
#pragma region necesary setup
	allocator = &heapMemory->allocator;
	new(mem) GameMemory; // *mem = GameMemory();
	console = &platformFunctions->console;

#pragma endregion


	//set the size of the window
	windowSettings->w = 640;
	windowSettings->h = 360;
	//windowSettings->drawWithOpenGl = true;
	//windowSettings->lockFpsIfNotVsync = 60;
	//windowSettings->vsyncWithOpengl = true;
	//gl2d::setVsync(1);

	windowSettings->drawWithOpenGl = false;


	mem->rayTracer.spheres.push_back({{-4,1,-5}, 1.f, glm::vec3(0.4,0.4,0.3), 0, 0.1});
	mem->rayTracer.spheres.push_back({{-0.3,1,-8}, 1.f, glm::vec3(0.6,0.3,0.34), 0.8, 0.1});
	mem->rayTracer.spheres.push_back({{4,1,-7}, 1.f, glm::vec3(0.2,0.6,0.24), 0, 0.5});
	mem->rayTracer.spheres.push_back({{4.3,-1,-4}, 0.5f, glm::vec3(0.6,0.1,0.64), 0.2, 0.8});
	mem->rayTracer.spheres.push_back({{-4.3,-1,4}, 1.f, glm::vec3(0.8,0.4,0.5), 0, 0.9});
	mem->rayTracer.spheres.push_back({{4.3,1,4}, 1.f, glm::vec3(0.8,0.4,0.9), 0.9, 0.9});
	mem->rayTracer.pointLights.push_back({glm::vec3(-10, 2, 2), 4.f});

	
}

//this might be usefull to change variables on runtime
extern "C" __declspec(dllexport) void onReload(GameMemory * mem, HeapMemory * heapMemory,
	WindowSettings * windowSettings, PlatformFunctions * platformFunctions)
{
#pragma region necesary setup
	allocator = &heapMemory->allocator;
	console = &platformFunctions->console;


#pragma endregion

	platformFunctions->console.log("reloaded...");

}


extern "C" __declspec(dllexport) void gameLogic(GameInput * input, GameMemory * mem,
	HeapMemory * heapMemory, VolatileMemory * volatileMemory, GameWindowBuffer * windowBuffer,
	WindowSettings * windowSettings, PlatformFunctions * platformFunctions)
{

#pragma region per frame setup
	allocator = &heapMemory->allocator;
	float deltaTime = input->deltaTime;
	console = &platformFunctions->console;


	float w = windowBuffer->w;
	float h = windowBuffer->h;

#pragma endregion

	static glm::vec3 pos = {};

	static float Yrotation = 0.f;
	static float Xrotation = 0.f;

	{
		glm::vec3 moveVector{};

		if (input->keyBoard[Button::W].held)
		{
			moveVector.z -= 1.f;
		}
		if (input->keyBoard[Button::S].held)
		{
			moveVector.z += 1.f;
		}
		if (input->keyBoard[Button::A].held)
		{
			moveVector.x -= 1.f;
		}
		if (input->keyBoard[Button::D].held)
		{
			moveVector.x += 1.f;
		}
		if (input->keyBoard[Button::R].held)
		{
			moveVector.y += 1.f;
		}
		if (input->keyBoard[Button::F].held)
		{
			moveVector.y -= 1.f;
		}

		moveVector *= deltaTime * 10;
	
		glm::vec3 lookVector = glm::rotate(Yrotation, glm::vec3(0, 1, 0)) * glm::vec4{0,0,-1, 0};
		glm::vec3 rightVector = glm::cross(lookVector, glm::vec3(0, 1, 0));

		glm::vec3 moveVectorViewSpace = lookVector * -moveVector.z;
		moveVectorViewSpace += rightVector * moveVector.x;
		moveVectorViewSpace += glm::vec3(0, 1, 0) * moveVector.y;

		pos += moveVectorViewSpace;

	}

	{
		constexpr float PI = 3.1415926;
		

		glm::vec2 mousePos = {input->mouseX, input->mouseY};
		static glm::vec2 lastMousePos = {};

		if (input->rightMouse.held)
		{
			glm::vec2 delta = mousePos - lastMousePos;
			delta.x /= w;
			delta.y /= h;
			
			delta *= deltaTime * PI * 20;

			Yrotation -= delta.x;
			Xrotation -= delta.y;

			if (Xrotation < -PI / 2.f)
			{
				Xrotation = -PI / 2.f;
			}
			if (Xrotation > PI / 2.f)
			{
				Xrotation = PI / 2.f;
			}
		}

		lastMousePos = mousePos;
	}

	float fov = glm::radians(60.f);
	float exposure = 1.0f;

	auto yRotationMatrix = glm::rotate(Yrotation, glm::vec3{0,1.f,0});
	auto xRotationMatrix = glm::rotate(Xrotation, glm::vec3{1.f,0.f,0});
	auto lookMatrix = yRotationMatrix * xRotationMatrix;


	for (int j = 0; j < h; j++)
	{
		for (int i = 0; i < w; i++)
		{
			float x = (2 * (i + 0.5) / (float)w - 1) * tan(fov / 2.) * w / (float)h;
			float y = -(2 * (j + 0.5) / (float)h - 1) * tan(fov / 2.);
			glm::vec3 dir = glm::normalize(glm::vec3(x, y, -1));
			dir = glm::normalize(lookMatrix * glm::vec4(dir,0));
			
			auto color = mem->rayTracer.renderRay(pos, dir, 5);

			//hdr tone mapping
			color = glm::vec3(1.f) - exp(-color * exposure);

			windowBuffer->drawAt(i, j, color.r, color.g, color.b);
			
		}
	}
	

}

extern "C" __declspec(dllexport) void onClose(GameMemory * mem, HeapMemory * heapMemory,
	WindowSettings * windowSettings, PlatformFunctions * platformFunctions)
{
#pragma region necesary setup
	allocator = &heapMemory->allocator;
	console = &platformFunctions->console;


#pragma endregion




}