#include "pch.h"

#include <Kinc/IO/FileReader.h>
#include <Kinc/Graphics4/Graphics.h>
#include <Kinc/Graphics4/IndexBuffer.h>
#include <Kinc/Graphics4/PipelineState.h>
#include <Kinc/Graphics4/Shader.h>
#include <Kinc/Graphics4/VertexBuffer.h>
#include <Kinc/System.h>

#include <assert.h>
#include <stdlib.h>

static kinc_g4_shader_t vertexShader;
static kinc_g4_shader_t fragmentShader;
static kinc_g4_pipeline_t pipeline;
static kinc_g4_vertex_buffer_t vertices;
static kinc_g4_index_buffer_t indices;

#define HEAP_SIZE 1024 * 1024
static uint8_t *heap = NULL;
static size_t heap_top = 0;

static void *allocate(size_t size) {
	size_t old_top = heap_top;
	heap_top += size;
	assert(heap_top <= HEAP_SIZE);
	return &heap[old_top];
}

static void update() {
	kinc_g4_begin(0);
	kinc_g4_clear(KINC_G4_CLEAR_COLOR, 0, 0.0f, 0);

	kinc_g4_set_pipeline(&pipeline);
	kinc_g4_vertex_buffer_t*vertexBuffers[1] = { &vertices };
	kinc_g4_set_vertex_buffers(vertexBuffers, 1);
	kinc_g4_set_index_buffer(&indices);
	kinc_g4_draw_indexed_vertices();

	kinc_g4_end(0);
	kinc_g4_swap_buffers();
}

static void loadShader(const char *filename, kinc_g4_shader_t *shader, kinc_g4_shader_type_t shader_type) {
	kinc_file_reader_t file;
	kinc_file_reader_open(&file, filename, KINC_FILE_TYPE_ASSET);
	size_t data_size = kinc_file_reader_size(&file);
	uint8_t *data = allocate(data_size);
	kinc_file_reader_read(&file, data, data_size);
	kinc_file_reader_close(&file);
	kinc_g4_shader_init(shader, data, data_size, shader_type);
}

int kore(int argc, char** argv) {
	kinc_init("Shader", 1024, 768, NULL, NULL);
	kinc_set_update_callback(update);

	heap = (uint8_t*)malloc(HEAP_SIZE);
	
	loadShader("shader.vert", &vertexShader, KINC_SHADER_TYPE_VERTEX);
	loadShader("shader.frag", &fragmentShader, KINC_SHADER_TYPE_FRAGMENT);
	
	kinc_g4_vertex_structure_t structure;
	kinc_g4_vertex_structure_init(&structure);
	kinc_g4_vertex_structure_add(&structure, "pos", KINC_G4_VERTEX_DATA_FLOAT3);
	kinc_g4_pipeline_init(&pipeline);
	pipeline.vertex_shader = &vertexShader;
	pipeline.fragment_shader = &fragmentShader;
	pipeline.input_layout[0] = &structure;
	pipeline.input_layout[1] = NULL;
	kinc_g4_pipeline_compile(&pipeline);

	kinc_g4_vertex_buffer_init(&vertices, 3, &structure, KINC_G4_USAGE_STATIC, 0);
	float *v = kinc_g4_vertex_buffer_lock_all(&vertices);
	v[0] = -1; v[1] = -1; v[2] = 0.5;
	v[3] = 1;  v[4] = -1; v[5] = 0.5;
	v[6] = -1; v[7] = 1;  v[8] = 0.5;
	kinc_g4_vertex_buffer_unlock_all(&vertices);

	kinc_g4_index_buffer_init(&indices, 3);
	int *i = kinc_g4_index_buffer_lock(&indices);
	i[0] = 0; i[1] = 1; i[2] = 2;
	kinc_g4_index_buffer_unlock(&indices);

	kinc_start();

	return 0;
}
