#include "VertexArray.h"
#include "Renderer.h"
#include "VertexBufferLayout.h"

void VertexBufferLayout::Push(unsigned int type, unsigned int count)
{
    std::cout << "Pushing element: type=" << type << ", count=" << count << std::endl;

    m_Elements.push_back(VertexBufferElement{type, count, static_cast<unsigned char>(type == GL_UNSIGNED_BYTE ? GL_TRUE : GL_FALSE)});
    m_Stride += count * VertexBufferElement::GetSizeOfType(type);

    // Debugging output
    for (const auto &element : m_Elements)
    {
        std::cout << "Stored Element: type=" << element.type
                  << ", count=" << element.count
                  << ", normalized=" << element.normalized
                  << std::endl;
    }
}

VertexArray::VertexArray()
{
    glGenVertexArrays(1, &m_RendererID);
    glBindVertexArray(m_RendererID);
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &m_RendererID);
}

void VertexArray::AddBuffer(const VertexBuffer &vb, const VertexBufferLayout &layout)
{
    Bind();
    vb.Bind();
    const auto &elements = layout.GetElements();
    unsigned int offset = 0;

    for (unsigned int i = 0; i < elements.size(); i++)
    {
        const auto &element = elements[i];
        std::cout << "Adding attribute " << i
                  << ": count=" << element.count
                  << ", type=" << element.type
                  << ", normalized=" << element.normalized
                  << ", stride=" << layout.GetStride()
                  << ", offset=" << offset
                  << std::endl;

        glEnableVertexAttribArray(i); // Enable the correct attribute index
        glVertexAttribPointer(i, element.count, element.type, element.normalized,
                              layout.GetStride(), reinterpret_cast<const void *>(offset));
        offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
    }
}

void VertexArray::Bind() const
{
    glBindVertexArray(m_RendererID);
}

void VertexArray::Unbind() const
{
    glBindVertexArray(0);
}
