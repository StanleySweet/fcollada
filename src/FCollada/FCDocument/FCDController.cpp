/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/
/*
	Based on the FS Import classes:
	Copyright (C) 2005-2006 Feeling Software Inc
	Copyright (C) 2005-2006 Autodesk Media Entertainment
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "FCDocument/FCDocument.h"
#include "FCDocument/FCDController.h"
#include "FCDocument/FCDSkinController.h"
#include "FCDocument/FCDMorphController.h"

//
// FCDController
//

ImplementObjectType(FCDController);
ImplementParameterObject(FCDController, FCDMorphController, morphController, new FCDMorphController(parent->GetDocument(), parent));
ImplementParameterObject(FCDController, FCDSkinController, skinController, new FCDSkinController(parent->GetDocument(), parent));

FCDController::FCDController(FCDocument* document)
:	FCDEntity(document, "Controller")
,	InitializeParameterNoArg(skinController)
,	InitializeParameterNoArg(morphController)
{
}

FCDController::~FCDController()
{
}


// Sets the type of this controller to a skin controller.
FCDSkinController* FCDController::CreateSkinController()
{
	morphController = nullptr;
	skinController = new FCDSkinController(GetDocument(), this);
	SetNewChildFlag();
	return skinController;
}

// Sets the type of this controller to a morph controller.
FCDMorphController* FCDController::CreateMorphController()
{
	skinController = nullptr;
	morphController = new FCDMorphController(GetDocument(), this);
	SetNewChildFlag();
	return morphController;
}

FCDEntity* FCDController::GetBaseTarget()
{
	if (skinController != nullptr) return skinController->GetTarget();
	else if (morphController != nullptr) return morphController->GetBaseTarget();
	else return nullptr;
}
const FCDEntity* FCDController::GetBaseTarget() const
{
	if (skinController != nullptr) return skinController->GetTarget();
	else if (morphController != nullptr) return morphController->GetBaseTarget();
	else return nullptr;
}

// Retrieves the base target geometry for this controller.
const FCDGeometry* FCDController::GetBaseGeometry() const
{
	const FCDEntity* base = GetBaseTarget();
	while (base != nullptr && base->GetType() == FCDEntity::CONTROLLER)
	{
		base = ((const FCDController*) base)->GetBaseTarget();
	}

	if (base != nullptr && base->GetType() == FCDEntity::GEOMETRY)
	{
		return (const FCDGeometry*) base;
	}
	return nullptr;
}

// Returns the first FCDController directly above the base of this controller
const FCDController* FCDController::GetBaseGeometryController() const
{
	const FCDEntity* parentBase = this;
	const FCDEntity* base = GetBaseTarget();
	while (base != nullptr && base->GetType() == FCDEntity::CONTROLLER)
	{
		parentBase = base;
		base = ((const FCDController*) base)->GetBaseTarget();
	}

	if (base != nullptr && base->GetType() == FCDEntity::GEOMETRY)
	{
		return (const FCDController*) parentBase;
	}

	return nullptr;
}
