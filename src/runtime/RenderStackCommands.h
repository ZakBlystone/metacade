/*
* <RenderStackCommands.h>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

REGISTER_RENDER_COMMAND(ColorData, Clear)
REGISTER_RENDER_COMMAND(ColorData, SetColor)
REGISTER_RENDER_COMMAND(ImageDrawData, Image)
REGISTER_RENDER_COMMAND(SpriteDrawData, Sprite)
REGISTER_RENDER_COMMAND(VoidData, PushTransform)
REGISTER_RENDER_COMMAND(VoidData, PopTransform)
REGISTER_RENDER_COMMAND(VoidData, Identity)
REGISTER_RENDER_COMMAND(VoidData, BlendAdd)
REGISTER_RENDER_COMMAND(VoidData, BlendNormal)
REGISTER_RENDER_COMMAND(VoidData, BlendMultiply)
REGISTER_RENDER_COMMAND(TransformMatrix, MatrixMult)