#include "pch.h"

#include <Kinc/IO/FileReader.h>
#include <Kinc/Graphics4/Graphics.h>
#include <Kinc/Graphics4/IndexBuffer.h>
#include <Kinc/Graphics4/PipelineState.h>
#include <Kinc/Graphics4/Shader.h>
#include <Kinc/Graphics4/VertexBuffer.h>
#include <Kinc/System.h>

#include <stdlib.h>

static Kinc_G4_Shader vertexShader;
static Kinc_G4_Shader fragmentShader;
static Kinc_G4_PipelineState pipeline;
static Kinc_G4_VertexBuffer vertices;
static Kinc_G4_IndexBuffer indices;
static uint8_t *heap;
static size_t heap_top;

static void update() {
	Kinc_G4_Begin(0);
	Kinc_G4_Clear(KINC_G4_CLEAR_COLOR, 0, 0.0f, 0);

	Kinc_G4_SetPipeline(&pipeline);
	Kinc_G4_VertexBuffer *vertexBuffers[1] = { &vertices };
	Kinc_G4_SetVertexBuffers(vertexBuffers, 1);
	Kinc_G4_SetIndexBuffer(&indices);
	Kinc_G4_DrawIndexedVertices();

	Kinc_G4_End(0);
	Kinc_G4_SwapBuffers();
}

int kore(int argc, char** argv) {
	Kinc_Init("Shader", 1024, 768, NULL, NULL);
	Kinc_SetUpdateCallback(update);

	heap = (uint8_t*)malloc(1024 * 1024);
	heap_top = 0;
	
	{
		Kinc_FileReader vs;
		Kinc_FileReader_Open(&vs, "shader.vert", KINC_FILE_TYPE_ASSET);
		uint8_t* vs_data = &heap[heap_top];
		size_t vs_data_size = Kinc_FileReader_Size(&vs);
		Kinc_FileReader_Read(&vs, vs_data, vs_data_size);
		Kinc_FileReader_Close(&vs);
		heap_top += vs_data_size;
		Kinc_G4_Shader_Create(&vertexShader, vs_data, vs_data_size, KINC_SHADER_TYPE_VERTEX);
	}

	{
		Kinc_FileReader fs;
		Kinc_FileReader_Open(&fs, "shader.frag", KINC_FILE_TYPE_ASSET);
		uint8_t* fs_data = &heap[heap_top];
		size_t fs_data_size = Kinc_FileReader_Size(&fs);
		Kinc_FileReader_Read(&fs, fs_data, fs_data_size);
		Kinc_FileReader_Close(&fs);
		heap_top += fs_data_size;
		Kinc_G4_Shader_Create(&fragmentShader, fs_data, fs_data_size, KINC_SHADER_TYPE_FRAGMENT);
	}
	
	Kinc_G4_VertexStructure structure;
	Kinc_G4_VertexStructure_Create(&structure);
	Kinc_G4_VertexStructure_Add(&structure, "pos", KINC_G4_VERTEX_DATA_FLOAT3);
	Kinc_G4_PipelineState_Create(&pipeline);
	pipeline.vertexShader = &vertexShader;
	pipeline.fragmentShader = &fragmentShader;
	pipeline.inputLayout[0] = &structure;
	pipeline.inputLayout[1] = NULL;
	Kinc_G4_PipelineState_Compile(&pipeline);

	Kinc_G4_VertexBuffer_Create(&vertices, 3, &structure, KINC_G4_USAGE_STATIC, 0);
	float *v = Kinc_G4_VertexBuffer_LockAll(&vertices);
	v[0] = -1; v[1] = -1; v[2] = 0.5;
	v[3] = 1;  v[4] = -1; v[5] = 0.5;
	v[6] = -1; v[7] = 1;  v[8] = 0.5;
	Kinc_G4_VertexBuffer_UnlockAll(&vertices);

	Kinc_G4_IndexBuffer_Create(&indices, 3);
	int *i = Kinc_G4_IndexBuffer_Lock(&indices);
	i[0] = 0; i[1] = 1; i[2] = 2;
	Kinc_G4_IndexBuffer_Unlock(&indices);

	Kinc_Start();

	return 0;
}
