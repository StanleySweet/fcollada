/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "FCDocument/FCDocument.h"
#include "FCDocument/FCDAnimated.h"
#include "FCDocument/FCDExtra.h"
#include "FColladaPlugin.h"


namespace FCollada { extern FColladaPluginManager* pluginManager; }

//
// FCDExtra
//

ImplementObjectType(FCDExtra);
ImplementParameterObject(FCDExtra, FCDEType, types, new FCDEType(parent->GetDocument(), parent, emptyCharString));

FCDExtra::FCDExtra(FCDocument* document, FUObject* _parent)
:	FCDObject(document)
,	parent(_parent)
,	InitializeParameterNoArg(types)
{
	// Create the default extra type.
	types.push_back(new FCDEType(document, this, emptyCharString));
	document->RegisterExtraTree(this);
}

FCDExtra::~FCDExtra()
{
	GetDocument()->UnregisterExtraTree(this);
}

// Adds a type of the given name (or return the existing type with this name).
FCDEType* FCDExtra::AddType(const char* name)
{
	FCDEType* type = FindType(name);
	if (type == nullptr)
	{
		type = new FCDEType(GetDocument(), this, emptyCharString);
		types.push_back(type);
		type->SetName(name);
		SetNewChildFlag();
	}
	return type;
}

// Search for a profile-specific type
const FCDEType* FCDExtra::FindType(const char* name) const
{
	for (const FCDEType** itT = types.begin(); itT != types.end(); ++itT)
	{
		if (IsEquivalent((*itT)->GetName(), name)) return *itT;
	}
	return nullptr;
}

bool FCDExtra::HasContent() const
{
	if (types.empty()) return false;
	for (const FCDEType** itT = types.begin(); itT != types.end(); ++itT)
	{
		size_t techniqueCount = (*itT)->GetTechniqueCount();
		for (size_t i = 0; i < techniqueCount; ++i)
		{
			const FCDETechnique* technique = (*itT)->GetTechnique(i);
			if (technique->GetChildNodeCount() > 0) return true;
		}
	}
	return false;
}

FCDExtra* FCDExtra::Clone(FCDExtra* clone) const
{
	if (clone == nullptr) clone = new FCDExtra(const_cast<FCDocument*>(GetDocument()), nullptr);

	// Create all the types
	clone->types.reserve(types.size());
	for (const FCDEType** itT = types.begin(); itT != types.end(); ++itT)
	{
		FCDEType* cloneT = clone->AddType((*itT)->GetName());
		(*itT)->Clone(cloneT);
	}
	return clone;
}

//
// FCDEType
//

ImplementObjectType(FCDEType);
ImplementParameterObject(FCDEType, FCDETechnique, techniques, new FCDETechnique(parent->GetDocument(), parent, emptyCharString));

FCDEType::FCDEType(FCDocument* document, FCDExtra* _parent, const char* _name)
:	FCDObject(document)
,	parent(_parent)
,	InitializeParameter(m_Name, _name)
,	InitializeParameterNoArg(m_Techniques)
{
}

FCDEType::~FCDEType()
{
}

// Adds a technique of the given profile (or return the existing technique with this profile).
FCDETechnique* FCDEType::AddTechnique(const char* profile)
{
	FCDETechnique* technique = FindTechnique(profile);
	if (technique == nullptr)
	{
		technique = new FCDETechnique(GetDocument(), this, profile);
		m_Techniques.push_back(technique);
		SetNewChildFlag();
	}
	return technique;
}

// Search for a profile-specific technique
const FCDETechnique* FCDEType::FindTechnique(const char* profile) const
{
	for (const FCDETechnique** itT = m_Techniques.begin(); itT != m_Techniques.end(); ++itT)
	{
		if (IsEquivalent((*itT)->GetProfile(), profile)) return *itT;
	}
	return nullptr;
}

// Search for a root node with a specific element name
const FCDENode* FCDEType::FindRootNode(const char* name) const
{
	const FCDENode* rootNode = nullptr;
	for (const FCDETechnique** itT = m_Techniques.begin(); itT != m_Techniques.end(); ++itT)
	{
		rootNode = (*itT)->FindChildNode(name);
		if (rootNode != nullptr) break;
	}
	return rootNode;
}

FCDEType* FCDEType::Clone(FCDEType* clone) const
{
	// If no clone is given: create one
	if (clone == nullptr)
	{
		clone = new FCDEType(const_cast<FCDocument*>(GetDocument()), nullptr, m_Name->c_str());
	}

	clone->m_Techniques.reserve(m_Techniques.size());
	for (const FCDETechnique** itT = m_Techniques.begin(); itT != m_Techniques.end(); ++itT)
	{
		FCDETechnique* cloneT = clone->AddTechnique((*itT)->GetProfile());
		(*itT)->Clone(cloneT);
	}
	return clone;
}

//
// FCDENode
//

ImplementObjectType(FCDENode);
ImplementParameterObject(FCDENode, FCDENode, children, new FCDENode(parent->GetDocument(), parent));
ImplementParameterObjectNoArg(FCDENode, FCDEAttribute, attributes);
ImplementParameterObject(FCDENode, FCDAnimatedCustom, animated, new FCDAnimatedCustom(parent->GetDocument()));

FCDENode::FCDENode(FCDocument* document, FCDENode* _parent)
:	FCDObject(document), parent(_parent)
,	InitializeParameterNoArg(m_Name)
,	InitializeParameterNoArg(m_Content)
,	InitializeParameterNoArg(m_Children)
,	InitializeParameterNoArg(m_Attributes)
,	InitializeParameterNoArg(m_Animated)
{
	m_Animated = new FCDAnimatedCustom(this);
}

FCDENode::~FCDENode()
{
	parent = nullptr;
}

void FCDENode::SetContent(const fchar* _content)
{
	// As COLLADA doesn't allow for mixed content, release all the children.
	while (!m_Children.empty())
	{
		m_Children.back()->Release();
	}

	m_Content = _content;
	SetDirtyFlag();
}

void FCDENode::SetAnimated(FCDAnimatedCustom* animatedCustom)
{ 
	SAFE_RELEASE(m_Animated); 
	m_Animated = animatedCustom;
}

// Search for a children with a specific name
const FCDENode* FCDENode::FindChildNode(const char* name) const
{
	for (const FCDENode* node : m_Children)
	{
		if (IsEquivalent(node->GetName(), name))
			return node;
	}
	return nullptr;
}

void FCDENode::FindChildrenNodes(const char* name, FCDENodeList& nodes) const
{
	for (const FCDENode* node : m_Children)
	{
		if(IsEquivalent(node->GetName(), name))
			nodes.push_back(node);
	}
}

const FCDENode* FCDENode::FindParameter(const char* name) const
{
	for (const FCDENode* node : m_Children)
	{
		if (IsEquivalent(node->GetName(), name)) return node;
	}
	return nullptr;
}

void FCDENode::FindParameters(FCDENodeList& nodes, StringList& names)
{
	for (const FCDENode* node : m_Children)
	{
		if (node->GetChildNodeCount() == 0)
		{
			nodes.push_back(node);
			names.push_back(node->GetName());
		}
	}
}

void FCDENode::SetName(fm::string& _name)
{
	m_Name = _name;
	CleanName(m_Name);
	SetDirtyFlag();
}

void FCDENode::CleanName(fm::string& n)
{
	size_t length = n.length();
	if (length == 0) return;

	// First character must be alphabetic or the underscore.
	if (n[0] != '_' && !(n[0] >= 'a' && n[0] <= 'z') && !(n[0] >= 'A' && n[0] <= 'Z'))
	{
		n[0] = '_';
	}

	// Other characters must be alpha-numeric or the underscore.
	for (size_t i = 1; i < length; ++i)
	{
		char& c = n[i];
		if (c != '_' && !(c >= 'a' && c <= 'z') && !(c >= 'A' && c <= 'Z') && !(c >= '0' && c <= '9'))
		{
			c = '_';
		}
	}
}

const fchar* FCDENode::GetContent() const
{
	return m_Content->c_str();
}

// Adds a new attribute to this extra tree node.
FCDEAttribute* FCDENode::AddAttribute(fm::string& _name, const fchar* _value)
{
	CleanName(_name);
	FCDEAttribute* attribute = FindAttribute(_name.c_str());
	if (attribute == nullptr)
	{
		attribute = new FCDEAttribute();
		m_Attributes.push_back(attribute);
		attribute->SetName(_name);
	}

	attribute->SetValue(_value);
	SetNewChildFlag();
	return attribute;
}

// Search for an attribute with a specific name
const FCDEAttribute* FCDENode::FindAttribute(const char* name) const
{
	for (const FCDEAttribute** itA = m_Attributes.begin(); itA != m_Attributes.end(); ++itA)
	{
		if (IsEquivalent((*itA)->GetName(), name)) return (*itA);
	}
	return nullptr;
}

const fstring& FCDENode::ReadAttribute(const char* name) const
{
	const FCDEAttribute* attribute = FindAttribute(name);
	return (attribute != nullptr) ? attribute->GetValue() : emptyFString;
}

FCDENode* FCDENode::AddParameter(const char* name, const fchar* value)
{
	FCDENode* parameter = AddChildNode();
	parameter->SetName(name);
	parameter->SetContent(value);
	SetNewChildFlag();
	return parameter;
}

FCDENode* FCDENode::Clone(FCDENode* clone) const
{
	if (clone == nullptr) return nullptr;

	clone->m_Name = m_Name;
	clone->m_Content = m_Content;

	clone->m_Attributes.reserve(m_Attributes.size());
	for (const FCDEAttribute** itA = m_Attributes.begin(); itA != m_Attributes.end(); ++itA)
	{
		clone->AddAttribute((*itA)->GetName(), (*itA)->GetValue());
	}

	clone->m_Children.reserve(m_Children.size());
	for (const FCDENode** itC = m_Children.begin(); itC != m_Children.end(); ++itC)
	{
		FCDENode* clonedChild = clone->AddChildNode();
		(*itC)->Clone(clonedChild);
	}

	// TODO: Clone the animated custom..

	return clone;
}

FCDENode* FCDENode::AddChildNode()
{
	FCDENode* node = new FCDENode(GetDocument(), this);
	m_Children.push_back(node);
	SetNewChildFlag(); 
	return node; 
}

FCDENode* FCDENode::AddChildNode(const char* name) 
{
	FCDENode* node = new FCDENode(GetDocument(), this);
	m_Children.push_back(node);
	node->SetName(name); 
	SetNewChildFlag();
	return node;
}

//
// FCDETechnique
//

ImplementObjectType(FCDETechnique);
ImplementParameterObjectNoCtr(FCDETechnique, FUObject, pluginOverride);

FCDETechnique::FCDETechnique(FCDocument* document, FCDEType* _parent, const char* _profile)
:	FCDENode(document, nullptr), parent(_parent)
,	InitializeParameterNoArg(pluginOverride)
,	InitializeParameter(profile, _profile)
{
}

FCDETechnique::~FCDETechnique() {}

FCDENode* FCDETechnique::Clone(FCDENode* clone) const
{
	if (clone == nullptr)
	{
		clone = new FCDETechnique(const_cast<FCDocument*>(GetDocument()), nullptr, profile->c_str());
	}
	else if (clone->GetObjectType().Includes(FCDETechnique::GetClassType()))
	{
		((FCDETechnique*) clone)->profile = profile;
	}

	FCDENode::Clone(clone);
	return clone;
}

//
// FCDEAttribute
//

FCDEAttribute::FCDEAttribute()
:	FUParameterizable()
,	InitializeParameterNoArg(name)
,	InitializeParameterNoArg(value)
{
}

FCDEAttribute::FCDEAttribute(const char* _name, const fchar* _value)
:	FUParameterizable()
,	InitializeParameter(name, _name)
,	InitializeParameter(value, _value)
{
}
