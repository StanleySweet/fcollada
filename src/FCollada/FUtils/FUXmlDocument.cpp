/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America

	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "FUXmlDocument.h"
#include "FUXmlWriter.h"
#include "FUFileManager.h"
#include "FUFile.h"
#include "FCDocument/FCDocument.h"

#define MAX_FILE_SIZE 10240000
//
// FUXmlDocument
//

FUXmlDocument::FUXmlDocument(FUFileManager* manager, const fchar* _filename, bool _isParsing)
:	isParsing(_isParsing), filename(_filename)
,	xmlDocument(nullptr)
{
	if (isParsing)
	{
		FUFile* file = nullptr;
		if (manager != nullptr) file = manager->OpenFile(filename, FUFile::READ);
		else file = new FUFile(filename, FUFile::READ);

		if (file->IsOpen())
		{
			size_t fileLength = file->GetLength();
			uint8* fileData = new uint8[fileLength];
			file->Read(fileData, fileLength);
			file->Close();

			// Open the given XML file.
			xmlDocument = xmlParseMemory((const char*) fileData, (int)fileLength);
			SAFE_DELETE_ARRAY(fileData);
		}
		SAFE_DELETE(file);
	}
	else
	{
		xmlDocument = xmlNewDoc(nullptr); // nullptr implies version 1.0.
	}
}

FUXmlDocument::FUXmlDocument(const char* data, size_t length)
:	isParsing(true)
,	xmlDocument(nullptr)
{
	FUAssert(data != nullptr, return);

	if (length == (size_t) ~0)
	{
		const char* c = data;
		for (length = 0; length < MAX_FILE_SIZE; ++length)
		{
			if (*(c++) == 0) break;
		}
		FUAssert(length < MAX_FILE_SIZE, return);
	}

	// Open the given XML file.
	xmlDocument = xmlParseMemory(data, (int)length);
}

FUXmlDocument::~FUXmlDocument()
{
	// Release the XML document
	ReleaseXmlData();
}

xmlNode* FUXmlDocument::GetRootNode()
{
	if (xmlDocument == nullptr)
	{
		return nullptr;
	}

	return xmlDocGetRootElement(xmlDocument);
}

xmlNode* FUXmlDocument::CreateRootNode(const char* name)
{
	xmlNode* rootNode = nullptr;
	if (!isParsing)
	{
		if (xmlDocument == nullptr) xmlDocument = xmlNewDoc(nullptr); // nullptr implies version 1.0.
		rootNode = FUXmlWriter::CreateNode(name);
		xmlDocSetRootElement(xmlDocument, rootNode);
	}
	return rootNode;
}

void FUXmlDocument::ReleaseXmlData()
{
	if (xmlDocument != nullptr)
	{
		if (!isParsing)
		{
		}
		xmlFreeDoc(xmlDocument);
		xmlDocument = nullptr;
	}
}

// Writes out the XML document.
bool FUXmlDocument::Write(const char* encoding)
{
	FUFile file(filename, FUFile::WRITE);
	if (!file.IsOpen()) return false;
	xmlDocument->encoding = xmlStrdup((const xmlChar*) encoding);
	return xmlDocFormatDump(file.GetHandle(), xmlDocument, 1) > 0;
}
