/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "FCDocument/FCDocument.h"
#include "FCDocument/FCDEntity.h"
#include "FCDocument/FCDPhysicsModel.h"
#include "FCDocument/FCDPhysicsModelInstance.h"
#include "FCDocument/FCDPhysicsRigidBody.h"
#include "FCDocument/FCDPhysicsRigidConstraint.h"
#include "FCDocument/FCDPhysicsRigidConstraintInstance.h"
#include "FUtils/FUUri.h"

ImplementObjectType(FCDPhysicsRigidConstraintInstance);

FCDPhysicsRigidConstraintInstance::FCDPhysicsRigidConstraintInstance(FCDocument* document, FCDPhysicsModelInstance* _parent, FCDPhysicsRigidConstraint* constraint)
:	FCDEntityInstance(document, nullptr, FCDEntity::PHYSICS_RIGID_CONSTRAINT), parent(_parent)
{
	if (constraint != nullptr)
	{
		SetRigidConstraint(constraint); 
	}
}

FCDPhysicsRigidConstraintInstance::~FCDPhysicsRigidConstraintInstance()
{
	parent = nullptr;
}

FCDEntityInstance* FCDPhysicsRigidConstraintInstance::Clone(FCDEntityInstance* _clone) const
{
	FCDPhysicsRigidConstraintInstance* clone = nullptr;
	if (_clone == nullptr) _clone = clone = new FCDPhysicsRigidConstraintInstance(const_cast<FCDocument*>(GetDocument()), nullptr, nullptr);
	else if (_clone->HasType(FCDPhysicsRigidConstraintInstance::GetClassType())) clone = (FCDPhysicsRigidConstraintInstance*) _clone;

	Parent::Clone(_clone);

	if (clone != nullptr)
	{
		// No interesting data to clone.
	}

	return _clone;
}

void FCDPhysicsRigidConstraintInstance::SetRigidConstraint(
		FCDPhysicsRigidConstraint* constraint)
{
	FUAssert(constraint != nullptr, ;);

	SetEntity(constraint); 
}
