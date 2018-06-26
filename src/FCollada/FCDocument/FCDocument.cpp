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
#include "FCDocument/FCDAnimated.h"
#include "FCDocument/FCDAnimation.h"
#include "FCDocument/FCDAnimationChannel.h"
#include "FCDocument/FCDAnimationClip.h"
#include "FCDocument/FCDAnimationCurve.h"
#include "FCDocument/FCDAsset.h"
#include "FCDocument/FCDCamera.h"
#include "FCDocument/FCDController.h"
#include "FCDocument/FCDEffect.h"
#include "FCDocument/FCDEmitter.h"
#include "FCDocument/FCDEntityReference.h"
#include "FCDocument/FCDExternalReferenceManager.h"
#include "FCDocument/FCDExtra.h"
#include "FCDocument/FCDForceField.h"
#include "FCDocument/FCDGeometry.h"
#include "FCDocument/FCDImage.h"
#include "FCDocument/FCDLight.h"
#include "FCDocument/FCDLibrary.h"
#include "FCDocument/FCDMaterial.h"
#include "FCDocument/FCDPlaceHolder.h"
#include "FCDocument/FCDPhysicsMaterial.h"
#include "FCDocument/FCDPhysicsModel.h"
#include "FCDocument/FCDPhysicsScene.h"
#include "FCDocument/FCDSceneNode.h"
#include "FCDocument/FCDTexture.h"
#include "FCDocument/FCDVersion.h"
#include "FUtils/FUFileManager.h"
#include "FUtils/FUUniqueStringMap.h"
#include "FUtils/FUDaeSyntax.h"

#include <iostream>

//
// FCDocument
//

ImplementObjectType(FCDocument);

ImplementParameterObject(FCDocument, FCDEntityReference, visualSceneRoot, new FCDEntityReference(parent->GetDocument(), nullptr));
ImplementParameterObject(FCDocument, FCDEntityReference, physicsSceneRoots, new FCDEntityReference(parent->GetDocument(), nullptr));
ImplementParameterObject(FCDocument, FCDAsset, asset, new FCDAsset(parent->GetDocument()));
ImplementParameterObject(FCDocument, FCDExtra, extra, new FCDExtra(parent->GetDocument(), parent));
ImplementParameterObject(FCDocument, FCDAnimationLibrary, animationLibrary, new FCDAnimationLibrary(parent->GetDocument()));
ImplementParameterObject(FCDocument, FCDAnimationClipLibrary, animationClipLibrary, new FCDAnimationClipLibrary(parent->GetDocument()));
ImplementParameterObject(FCDocument, FCDCameraLibrary, cameraLibrary, new FCDCameraLibrary(parent->GetDocument()));
ImplementParameterObject(FCDocument, FCDControllerLibrary, controllerLibrary, new FCDControllerLibrary(parent->GetDocument()));
ImplementParameterObject(FCDocument, FCDEffectLibrary, effectLibrary, new FCDEffectLibrary(parent->GetDocument()));
ImplementParameterObject(FCDocument, FCDForceFieldLibrary, forceFieldLibrary, new FCDForceFieldLibrary(parent->GetDocument()));
ImplementParameterObject(FCDocument, FCDGeometryLibrary, geometryLibrary, new FCDGeometryLibrary(parent->GetDocument()));
ImplementParameterObject(FCDocument, FCDImageLibrary, imageLibrary, new FCDImageLibrary(parent->GetDocument()));
ImplementParameterObject(FCDocument, FCDLightLibrary, lightLibrary, new FCDLightLibrary(parent->GetDocument()));
ImplementParameterObject(FCDocument, FCDMaterialLibrary, materialLibrary, new FCDMaterialLibrary(parent->GetDocument()));
ImplementParameterObject(FCDocument, FCDPhysicsModelLibrary, physicsModelLibrary, new FCDPhysicsModelLibrary(parent->GetDocument()));
ImplementParameterObject(FCDocument, FCDPhysicsMaterialLibrary, physicsMaterialLibrary, new FCDPhysicsMaterialLibrary(parent->GetDocument()));
ImplementParameterObject(FCDocument, FCDPhysicsSceneLibrary, physicsSceneLibrary, new FCDPhysicsSceneLibrary(parent->GetDocument()));
ImplementParameterObject(FCDocument, FCDVisualSceneNodeLibrary, visualSceneLibrary, new FCDVisualSceneNodeLibrary(parent->GetDocument()));
ImplementParameterObject(FCDocument, FCDEmitterLibrary, emitterLibrary, new FCDEmitterLibrary(parent->GetDocument()));

FCDocument::FCDocument()
:	FCDObject(this)
,	fileManager(nullptr), version(nullptr), uniqueNameMap(nullptr)
,	InitializeParameterNoArg(visualSceneRoot)
,	InitializeParameterNoArg(physicsSceneRoots)
,	InitializeParameterNoArg(asset)
,	InitializeParameterNoArg(extra)
,	hasStartTime(false), hasEndTime(false)
,	InitializeParameter(startTime, 0.0f)
,	InitializeParameter(endTime, 0.0f)
,	InitializeParameterNoArg(animationLibrary)
,	InitializeParameterNoArg(animationClipLibrary)
,	InitializeParameterNoArg(cameraLibrary)
,	InitializeParameterNoArg(controllerLibrary)
,	InitializeParameterNoArg(effectLibrary)
,	InitializeParameterNoArg(forceFieldLibrary)
,	InitializeParameterNoArg(geometryLibrary)
,	InitializeParameterNoArg(imageLibrary)
,	InitializeParameterNoArg(lightLibrary)
,	InitializeParameterNoArg(materialLibrary)
,	InitializeParameterNoArg(physicsModelLibrary)
,	InitializeParameterNoArg(physicsMaterialLibrary)
,	InitializeParameterNoArg(physicsSceneLibrary)
,	InitializeParameterNoArg(visualSceneLibrary)
,	InitializeParameterNoArg(emitterLibrary)
{
	DEBUG_OUT("In ctor");
	fileManager = new FUFileManager();
	version = new FCDVersion(DAE_SCHEMA_VERSION);
	uniqueNameMap = new FUSUniqueStringMap();

	asset = new FCDAsset(this);
	externalReferenceManager = new FCDExternalReferenceManager(this);
	extra = new FCDExtra(this, this);
	visualSceneRoot = new FCDEntityReference(this, nullptr);

	animationLibrary = new FCDAnimationLibrary(this);
	animationClipLibrary = new FCDAnimationClipLibrary(this);
	cameraLibrary = new FCDCameraLibrary(this);
	controllerLibrary = new FCDControllerLibrary(this);
	emitterLibrary = new FCDEmitterLibrary(this);
	forceFieldLibrary = new FCDForceFieldLibrary(this);
	effectLibrary = new FCDEffectLibrary(this);
	geometryLibrary = new FCDGeometryLibrary(this);
	imageLibrary = new FCDImageLibrary(this);
	lightLibrary = new FCDLightLibrary(this);
	materialLibrary = new FCDMaterialLibrary(this);
	visualSceneLibrary = new FCDVisualSceneNodeLibrary(this);
	physicsMaterialLibrary = new FCDPhysicsMaterialLibrary(this);
	physicsModelLibrary = new FCDPhysicsModelLibrary(this);
	physicsSceneLibrary = new FCDPhysicsSceneLibrary(this);
}

FCDocument::~FCDocument()
{
	// Release the external references to and from this document
	// before all clearing the entities.
	FUTrackable::Detach();
	DEBUG_OUT("In dtor");
	externalReferenceManager = nullptr;

	// Release the libraries and the asset
	animationLibrary = nullptr;
	animationClipLibrary = nullptr;
	cameraLibrary = nullptr;
	controllerLibrary = nullptr;
	effectLibrary = nullptr;
	emitterLibrary = nullptr;
	forceFieldLibrary = nullptr;
	geometryLibrary = nullptr;
	imageLibrary = nullptr;
	lightLibrary = nullptr;
	materialLibrary = nullptr;
	visualSceneLibrary = nullptr;
	physicsMaterialLibrary = nullptr;
	physicsModelLibrary = nullptr;
	physicsSceneLibrary = nullptr;
	asset = nullptr;

	// Must be released last
	CLEAR_POINTER_VECTOR(layers);
	while (!animatedValues.empty()) { animatedValues.begin()->first->Release(); }
//	animatedValueMap.clear();

	SAFE_DELETE(fileManager);
	SAFE_DELETE(uniqueNameMap);
	SAFE_DELETE(version);
}

const FCDSceneNode* FCDocument::GetVisualSceneInstance() const
{
	if (visualSceneRoot == nullptr) return nullptr;
	const FCDEntity* entity = visualSceneRoot->GetEntity();
	if (entity != nullptr && entity->HasType(FCDSceneNode::GetClassType())) return (const FCDSceneNode*) entity;
	else return nullptr;
}

const FCDPhysicsScene* FCDocument::GetPhysicsSceneInstance(size_t index) const
{
	if (index == 0 && physicsSceneRoots.empty()) return nullptr;
	FUAssert(index < physicsSceneRoots.size(), return nullptr);
	const FCDEntity* entity = physicsSceneRoots[index]->GetEntity();
	if (entity != nullptr && entity->HasType(FCDPhysicsScene::GetClassType())) return (const FCDPhysicsScene*) entity;
	else return nullptr;
}

FCDEntityReference* FCDocument::AddPhysicsSceneInstanceReference()
{
	physicsSceneRoots.push_back(new FCDEntityReference(this, (FCDObjectWithId*) nullptr));
	return physicsSceneRoots.back();
}

void FCDocument::AddPhysicsSceneInstance(FCDPhysicsScene* scene)
{
	FCDEntityReference* reference = AddPhysicsSceneInstanceReference();
	reference->SetEntity(scene);
}

// Adds an entity layer to the document.
FCDLayer* FCDocument::AddLayer()
{
	FCDLayer* layer = new FCDLayer();
	layers.push_back(layer);
	return layer;
}

// Releases an entity layer from the document.
void FCDocument::ReleaseLayer(FCDLayer* layer)
{
	layers.release(layer);
}

// Search for a specific COLLADA library items with a given COLLADA id.
FCDAnimation* FCDocument::FindAnimation(const fm::string& daeId) { return animationLibrary->FindDaeId(daeId); }
FCDAnimationClip* FCDocument::FindAnimationClip(const fm::string& daeId) { return animationClipLibrary->FindDaeId(daeId); }
FCDCamera* FCDocument::FindCamera(const fm::string& daeId) { return cameraLibrary->FindDaeId(daeId); }
FCDController* FCDocument::FindController(const fm::string& daeId) { return controllerLibrary->FindDaeId(daeId); }
FCDEffect* FCDocument::FindEffect(const fm::string& daeId) { return effectLibrary->FindDaeId(daeId); }
FCDEmitter* FCDocument::FindEmitter(const fm::string& daeId) { return emitterLibrary->FindDaeId(daeId); }
FCDForceField* FCDocument::FindForceField(const fm::string& daeId) { return forceFieldLibrary->FindDaeId(daeId); }
FCDGeometry* FCDocument::FindGeometry(const fm::string& daeId) { return geometryLibrary->FindDaeId(daeId); }
FCDImage* FCDocument::FindImage(const fm::string& daeId) { return imageLibrary->FindDaeId(daeId); }
FCDLayer* FCDocument::FindLayer(const fm::string& name) { for (FCDLayerList::iterator itL = layers.begin(); itL != layers.end(); ++itL) { if ((*itL)->name == name) return *itL; } return nullptr; }
FCDLight* FCDocument::FindLight(const fm::string& daeId) { return lightLibrary->FindDaeId(daeId); }
FCDMaterial* FCDocument::FindMaterial(const fm::string& daeId) { return  materialLibrary->FindDaeId(daeId); }
FCDSceneNode* FCDocument::FindVisualScene(const fm::string& daeId) { return visualSceneLibrary->FindDaeId(daeId); }
FCDPhysicsScene* FCDocument::FindPhysicsScene(const fm::string& daeId) { return physicsSceneLibrary->FindDaeId(daeId); }
FCDPhysicsMaterial* FCDocument::FindPhysicsMaterial(const fm::string& daeId) { return physicsMaterialLibrary->FindDaeId(daeId); }
FCDPhysicsModel* FCDocument::FindPhysicsModel(const fm::string& daeId) { return physicsModelLibrary->FindDaeId(daeId); }
const FCDSceneNode* FCDocument::FindSceneNode(const char* daeId) const { return visualSceneLibrary->FindDaeId(daeId); }
FCDEntity* FCDocument::FindEntity(const fm::string& daeId)
{
#define CHECK_LIB(libraryName) { \
	FCDEntity* e = libraryName->FindDaeId(daeId); \
	if (e != nullptr) return e; }

	CHECK_LIB(animationLibrary);
	CHECK_LIB(animationClipLibrary);
	CHECK_LIB(cameraLibrary);
	CHECK_LIB(controllerLibrary);
	CHECK_LIB(effectLibrary);
	CHECK_LIB(emitterLibrary);
	CHECK_LIB(forceFieldLibrary);
	CHECK_LIB(geometryLibrary);
	CHECK_LIB(imageLibrary);
	CHECK_LIB(lightLibrary);
	CHECK_LIB(materialLibrary);
	CHECK_LIB(visualSceneLibrary);
	CHECK_LIB(physicsSceneLibrary);
	CHECK_LIB(physicsMaterialLibrary);
	CHECK_LIB(physicsModelLibrary);
#undef CHECK_LIB

	return nullptr;
}

// Add an animated value to the list
void FCDocument::RegisterAnimatedValue(FCDAnimated* animated)
{
	// Look for a duplicate in order to avoid memory loss
	//if (animated->GetValueCount() == 0 || FindAnimatedValue(animated->GetValue(0)) != nullptr)
	if (animated->GetValueCount() == 0)
	{
		SAFE_RELEASE(animated);
		return;
	}

	// List the new animated value
	animatedValues.insert(animated, animated);

	//// Also add to the map the individual values for easy retrieval
	//size_t count = animated->GetValueCount();
	//for (size_t i = 0; i < count; ++i)
	//{
	//	const float* value = animated->GetValue(i);
	//	animatedValueMap.insert(value, animated);
	//}
}

// Unregisters an animated value of the document.
void FCDocument::UnregisterAnimatedValue(FCDAnimated* animated)
{
	if (animated != nullptr)
	{
		// Intentionally search from the end:
		// - In the destructor of the document, we delete from the end.
		// - In animation exporters, we add to the end and are likely to delete right away.
		FCDAnimatedSet::iterator it = animatedValues.find(animated);
		if (it != animatedValues.end())
		{
			animatedValues.erase(it);

			//// Also remove to the map the individual values contained
			//size_t count = animated->GetValueCount();
			//for (size_t i = 0; i < count; ++i)
			//{
			//	const float* value = animated->GetValue(i);
			//	FCDAnimatedValueMap::iterator itV = animatedValueMap.find(value);
			//	if (itV != animatedValueMap.end() && (*itV).second == animated)
			//	{
			//		animatedValueMap.erase(itV);
			//	}
			//}
		}
	}
}
//
//// Retrieve an animated value, given a value pointer
//const FCDAnimated* FCDocument::FindAnimatedValue(const float* ptr) const
//{
//	FCDAnimatedValueMap::const_iterator it = animatedValueMap.find(ptr);
//	return (it != animatedValueMap.end()) ? (*it).second : nullptr;
//}
//
//// Retrieve an animated float value for a given fully qualified target
//const float* FCDocument::FindAnimatedTarget(const fm::string& fullyQualifiedTarget)
//{
//	if (fullyQualifiedTarget.empty()) return nullptr;
//	fm::string target = (fullyQualifiedTarget[0] == '#') ? fullyQualifiedTarget.substr(1) : fullyQualifiedTarget;
//	fm::string pointer, qualifier;
//	FUStringConversion::SplitTarget(target, pointer, qualifier);
//
//	// Find the pointer
//	FCDAnimated* animatedValue = nullptr;
//	for (FCDAnimatedSet::iterator itA = animatedValues.begin(); itA != animatedValues.end(); ++itA)
//	{
//		FCDAnimated* animated = (*itA).first;
//		if (animated->GetTargetPointer() == pointer) { animatedValue = animated; break; }
//	}
//	if (animatedValue == nullptr) return nullptr;
//
//	// Return the qualified value
//	size_t index = animatedValue->FindQualifier(qualifier);
//	if (index == size_t(-1)) return nullptr;
//	return animatedValue->GetValue(index);
//}
//
//// Returns whether a given value pointer is animated
//bool FCDocument::IsValueAnimated(const float* ptr) const
//{
//	const FCDAnimated* animated = FindAnimatedValue(ptr);
//	return (animated != nullptr) ? animated->HasCurve() : false;
//}

// Insert new library elements
FCDSceneNode* FCDocument::AddVisualScene()
{
	FCDSceneNode* visualScene = visualSceneLibrary->AddEntity();
	if (visualSceneRoot->GetEntity() == nullptr) visualSceneRoot->SetEntity(visualScene);
	return visualScene;
}
FCDPhysicsScene* FCDocument::AddPhysicsScene()
{
	FCDPhysicsScene* physicsScene = physicsSceneLibrary->AddEntity();
	AddPhysicsSceneInstance(physicsScene);
	return physicsScene;
}

void FCDocument::SetFileUrl(const fstring& filename)
{
	fileManager->PopRootFile();
	fileUrl = GetFileManager()->GetCurrentUri().MakeAbsolute(filename);
	fileManager->PushRootFile(fileUrl);
}

void FCDocument::SetCurrentTime(float time)
{
	for (FCDAnimatedSet::iterator itA = animatedValues.begin(); itA != animatedValues.end(); ++itA)
	{
		FCDAnimated* animated = (*itA).first;
		animated->Evaluate(time);
	}

	// We must evaluate all our child documents as well!
	for (size_t i = 0; i < externalReferenceManager->GetPlaceHolderCount(); i++)
	{
		FCDPlaceHolder* placeHolder = externalReferenceManager->GetPlaceHolder(i);
		if (placeHolder->IsTargetLoaded()) placeHolder->GetTarget()->SetCurrentTime(time);
	}
}

// More linker-tricking for DLL support.
extern bool CreateLibraryFunctions(FCDocument* doc)
{
	doc->GetForceFieldLibrary()->GetEntityCount();
	doc->GetForceFieldLibrary()->GetEntity(0);
	doc->GetImageLibrary()->GetEntityCount();
	doc->GetImageLibrary()->GetEntity(0);
	doc->GetPhysicsMaterialLibrary()->GetEntityCount();
	doc->GetPhysicsMaterialLibrary()->GetEntity(0);
	doc->GetPhysicsModelLibrary()->GetEntityCount();
	doc->GetPhysicsModelLibrary()->GetEntity(0);
	return true;
}
