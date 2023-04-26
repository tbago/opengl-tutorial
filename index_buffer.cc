#include "index_buffer.h"

#include <GLFW/glfw3.h>
#include <cassert>

#include "renderer.h"

IndexBuffer::IndexBuffer(const unsigned int *data, unsigned int count)
    : count_(count) {
    assert(sizeof(unsigned int) == sizeof(GLuint));

    GLCall(glGenBuffers(1, &renderer_id_));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer_id_));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int),
                        data, GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer() { GLCall(glDeleteBuffers(1, &renderer_id_)); }

void IndexBuffer::Bind() const {
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer_id_));
}

void IndexBuffer::Unbind() const {
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer_id_));
}
