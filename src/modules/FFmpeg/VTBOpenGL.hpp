/*
    QMPlay2 is a video and audio player.
    Copyright (C) 2010-2025  Błażej Szczygieł

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <opengl/OpenGLHWInterop.hpp>

struct AVBufferRef;

class VTBOpenGL final : public OpenGLHWInterop
{
public:
    VTBOpenGL(AVBufferRef *hwDeviceBufferRef);
    ~VTBOpenGL();

    QString name() const override;

    Format getFormat() const override;
    bool isTextureRectangle() const override;
    bool isCopy() const override;

    bool init(const int *widths, const int *heights, const SetTextureParamsFn &setTextureParamsFn) override;
    void clear() override;

    bool mapFrame(Frame &videoFrame) override;
    quint32 getTexture(int plane) override;

    Frame getCpuFrame(const Frame &videoFrame) override;

public:
    AVBufferRef *m_hwDeviceBufferRef = nullptr;

private:
    quint32 m_textures[2] = {};

    int m_widths[2] = {};
    int m_heights[2] = {};
};
