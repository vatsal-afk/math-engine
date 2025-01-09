#include "VertexBuffer.h"
#include "Renderer.h"

VertexBuffer::VertexBuffer(const void* data, unsigned int size)
{
    glGenBuffers(1, &m_RendererID); // an id for the object, hence the pointer
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    //the line below links the buffer with the vao
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW); // this size is in bytes
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &m_RendererID);
}

void VertexBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void VertexBuffer::Unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}