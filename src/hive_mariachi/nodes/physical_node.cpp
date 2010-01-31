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

#include "stdafx.h"

#include "physical_node.h"

using namespace mariachi;

/**
* Constructor of the class.
*/
PhysicalNode::PhysicalNode() : CubeNode() {
}

PhysicalNode::PhysicalNode(const std::string &name) : CubeNode(name) {
}

/**
* Destructor of the class.
*/
PhysicalNode::~PhysicalNode() {
}

void PhysicalNode::addImpulse(const Coordinate3d_t &impulse) {
}

void PhysicalNode::addTorque(const Coordinate3d_t &torque) {
}

float PhysicalNode::getMass() {
    return this->mass;
}

void PhysicalNode::setMass(float mass) {
    this->mass = mass;
}

Coordinate3d_t &PhysicalNode::getInertia() {
    return this->inertia;
}

void PhysicalNode::setInertia(const Coordinate3d_t &inertia) {
    this->inertia = inertia;
}
