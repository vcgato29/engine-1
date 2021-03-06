/*
 * Copyright (C) 2015 EEnginE project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec3 iVertex;
layout (location = 1) in vec3 iNormals;

layout (binding = 0) uniform UBuffer {
   mat4 view;
} uBuff;

layout(push_constant) uniform PBlock {
   mat4 model;
} pConst;

layout (location = 0) out vec3 vColor;

void main() {
   vColor = normalize( iNormals );
   gl_Position = uBuff.view * pConst.model * vec4(iVertex, 1.0);
}
