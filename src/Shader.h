#pragma once

class Shader
{
    private:
    unsigned int m_RendererID;
    //caching for uniforms
    public:
    Shader(const std::string& filepath);
    ~Shader();

    void Bind() const;
    void Unvind() const;

    void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
    private:
    unsigned int GetUniformLocation(const std::string& name);
};