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
#include "FCDocument/FCDAsset.h"
#include "FCDocument/FCDEntity.h"
#include "FCDocument/FCDExtra.h"

//
// Constants
//

static const size_t MAX_NAME_LENGTH = 512;

//
// FCDEntity
//

ImplementObjectType(FCDEntity);
ImplementParameterObject(FCDEntity, FCDAsset, asset, new FCDAsset(parent->GetDocument()));
ImplementParameterObject(FCDEntity, FCDExtra, extra, new FCDExtra(parent->GetDocument(), parent));

FCDEntity::FCDEntity(FCDocument* document, const char* baseId)
:	FCDObjectWithId(document, baseId)
,	InitializeParameterNoArg(m_Name)
,	InitializeParameterNoArg(m_Extra)
,	InitializeParameterNoArg(m_Asset)
,	InitializeParameterNoArg(m_Note)
{
	m_Extra = new FCDExtra(document, this);
}

FCDEntity::~FCDEntity()
{
}

// Structure cloning
FCDEntity* FCDEntity::Clone(FCDEntity* clone, bool UNUSED(cloneChildren)) const
{
	if (clone == nullptr)
	{
		clone = new FCDEntity(const_cast<FCDocument*>(GetDocument()));
	}

	FCDObjectWithId::Clone(clone);
	clone->m_Name = m_Name;
	clone->m_Note = m_Note;
	if (m_Extra != nullptr)
	{
		m_Extra->Clone(clone->m_Extra);
	}
	return clone;
}

fstring FCDEntity::CleanName(const fchar* c)
{
	size_t len = 0;
	for (; len < MAX_NAME_LENGTH; len++) { if (c[len] == 0) break; }
	fstring cleanName(len, *c);
	fchar* id = cleanName.begin();
	if (*c != 0)
	{
	
		// First character: alphabetic or '_'.
		if ((*c >= 'a' && *c <= 'z') || (*c >= 'A' && *c <= 'Z') || *c == '_') *id = *c;
		else *id = '_';

		// Other characters: alphabetic, numeric, '_'.
		// NOTE: ':' and '.' are NOT acceptable characters.
		for (size_t i = 1; i < len; ++i)
		{
			++id; ++c;
			if ((*c >= 'a' && *c <= 'z') || (*c >= 'A' && *c <= 'Z') || (*c >= '0' && *c <= '9') || *c == '_' || *c == '-') *id = *c;
			else *id = '_';
		}
		*(++id) = 0;
	}
	return cleanName;
}

void FCDEntity::SetName(const fstring& _name) 
{
	m_Name = CleanName(_name.c_str());
	SetDirtyFlag();
}

FCDAsset* FCDEntity::GetAsset()
{
	return (m_Asset != nullptr) ? m_Asset : (m_Asset = new FCDAsset(GetDocument()));
}

void FCDEntity::GetHierarchicalAssets(FCDAssetConstList& assets) const
{
	if (m_Asset != nullptr) assets.push_back(m_Asset);
	else assets.push_back(GetDocument()->GetAsset());
}

// Look for a children with the given COLLADA Id.
const FCDEntity* FCDEntity::FindDaeId(const fm::string& _daeId) const
{
	if (GetDaeId() == _daeId) return this;
	return nullptr;
}

bool FCDEntity::HasNote() const
{
	return !m_Note->empty();
}

const fstring& FCDEntity::GetNote() const
{
	return *m_Note;
}

void FCDEntity::SetNote(const fstring& _note)
{
	m_Note = _note;
	SetDirtyFlag();
}
