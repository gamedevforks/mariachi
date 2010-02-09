// Hive Mariachi Engine
// Copyright (C) 2008 Hive Solutions Lda.
//
// This file is part of Hive Mariachi Engine.
//
// Hive Mariachi Engine is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Hive Mariachi Engine is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Hive Mariachi Engine. If not, see <http://www.gnu.org/licenses/>.

// __author__    = Jo�o Magalh�es <joamag@hive.pt>
// __version__   = 1.0.0
// __revision__  = $LastChangedRevision$
// __date__      = $LastChangedDate$
// __copyright__ = Copyright (c) 2008 Hive Solutions Lda.
// __license__   = GNU General Public License (GPL), Version 3

#pragma once

#include "../structures/mesh.h"

#include "physical_node.h"

namespace mariachi {
    namespace nodes {
        class ModelNode : public PhysicalNode {
            private:
                std::vector<structures::Mesh_t *> *meshList;
                Texture *texture;

            public:
                ModelNode();
                ModelNode(const std::string &name);
                ~ModelNode();
                inline void initRenderable();
                virtual std::vector<structures::Mesh_t *> *getMeshList();
                virtual void setMeshList(std::vector<structures::Mesh_t *> *meshList);
                virtual Texture *getTexture();
                virtual void setTexture(Texture *texture);
                virtual inline unsigned int getNodeType() { return MODEL_NODE_TYPE; };
        };
    }
}
