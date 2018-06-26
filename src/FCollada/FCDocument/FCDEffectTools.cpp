/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America

	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "FCDocument/FCDocument.h"
#include "FCDocument/FCDEffect.h"
#include "FCDocument/FCDEffectStandard.h"
#include "FCDocument/FCDEffectParameter.h"
#include "FCDocument/FCDEffectProfileFX.h"
#include "FCDocument/FCDEffectTools.h"
#include "FCDocument/FCDEffectTechnique.h"
#include "FCDocument/FCDMaterialInstance.h"
#include "FCDocument/FCDGeometryInstance.h"
#include "FCDocument/FCDMaterial.h"
#include "FCDocument/FCDTexture.h"
#include "FUtils/FUDaeEnum.h"

//
// FCDEffectTools
//

namespace FCDEffectTools
{
	void SynchronizeAnimatedParams(FCDGeometryInstance* geometryInstance, FCDMaterialInstance* materialInstance)
	{
		//Find the different classes that are needed.
		FUAssert(geometryInstance != nullptr && materialInstance != nullptr, return);
		FCDMaterial* material = materialInstance->GetMaterial();
		FUAssert(material != nullptr, return);
		FCDEffect* effect = material->GetEffect();
		FUAssert(effect != nullptr, return);
		FCDEffectProfile* effectProfile = effect->FindProfile(FUDaeProfileType::COMMON);
		if (!effectProfile) return;
		FCDEffectStandard* effectStandard = (FCDEffectStandard*) effectProfile;

		//Synchronize each parameter from the FCDEffectStandard class.
		if (effectStandard->GetEmissionColorParam()->IsReferencer())
			LinkAnimatedParamCommonVector(FCDEffectStandard::EmissionColorSemantic, effectStandard->GetEmissionColorParam(), geometryInstance, material, effect, effectProfile);
		if (effectStandard->GetEmissionFactorParam()->IsReferencer())
			LinkAnimatedParamCommonFloat(FCDEffectStandard::EmissionFactorSemantic, effectStandard->GetEmissionFactorParam(), geometryInstance, material, effect, effectProfile);
		if (effectStandard->GetReflectivityColorParam()->IsReferencer())
			LinkAnimatedParamCommonVector(FCDEffectStandard::ReflectivityColorSemantic, effectStandard->GetReflectivityColorParam(), geometryInstance, material, effect, effectProfile);
		if (effectStandard->GetReflectivityFactorParam()->IsReferencer())
			LinkAnimatedParamCommonFloat(FCDEffectStandard::ReflectivityFactorSemantic, effectStandard->GetReflectivityFactorParam(), geometryInstance, material, effect, effectProfile);
		if (effectStandard->GetIndexOfRefractionParam()->IsReferencer())
			LinkAnimatedParamCommonFloat(FCDEffectStandard::IndexOfRefractionSemantic, effectStandard->GetIndexOfRefractionParam(), geometryInstance, material, effect, effectProfile);
		if (effectStandard->GetTranslucencyColorParam()->IsReferencer())
			LinkAnimatedParamCommonVector(FCDEffectStandard::TranslucencyColorSemantic, effectStandard->GetTranslucencyColorParam(), geometryInstance, material, effect, effectProfile);
		if (effectStandard->GetTranslucencyFactorParam()->IsReferencer())
			LinkAnimatedParamCommonFloat(FCDEffectStandard::TranslucencyFactorSemantic, effectStandard->GetTranslucencyFactorParam(), geometryInstance, material, effect, effectProfile);
		if (effectStandard->GetDiffuseColorParam()->IsReferencer())
			LinkAnimatedParamCommonVector(FCDEffectStandard::DiffuseColorSemantic, effectStandard->GetDiffuseColorParam(), geometryInstance, material, effect, effectProfile);
		if (effectStandard->GetAmbientColorParam()->IsReferencer())
			LinkAnimatedParamCommonVector(FCDEffectStandard::AmbientColorSemantic, effectStandard->GetAmbientColorParam(), geometryInstance, material, effect, effectProfile);
		if (effectStandard->GetSpecularColorParam()->IsReferencer())
			LinkAnimatedParamCommonVector(FCDEffectStandard::SpecularColorSemantic, effectStandard->GetSpecularColorParam(), geometryInstance, material, effect, effectProfile);
		if (effectStandard->GetSpecularFactorParam()->IsReferencer())
			LinkAnimatedParamCommonFloat(FCDEffectStandard::SpecularFactorSemantic, effectStandard->GetSpecularFactorParam(), geometryInstance, material, effect, effectProfile);
		if (effectStandard->GetShininessParam()->IsReferencer())
			LinkAnimatedParamCommonFloat(FCDEffectStandard::ShininessSemantic, effectStandard->GetShininessParam(), geometryInstance, material, effect, effectProfile);
	}

	void LinkAnimatedParamCommonFloat(const fm::string& semantic, FCDEffectParameterFloat* param, FCDGeometryInstance* geometry, FCDMaterial* material, FCDEffect* effect, FCDEffectProfile* profile)
	{
		const fm::string& reference = param->GetReference();
		if (reference.empty()) return;
		FCDEffectParameter* geometryParam = FindEffectParameterBySemantic(geometry, semantic);
		FCDEffectParameter* materialParam = FindEffectParameterByReference(material, reference, true);
		FCDEffectParameter* effectParam = FindEffectParameterByReference(effect, reference, true);
		FCDEffectParameter* profileParam = FindEffectParameterByReference(profile, reference, false);
		if (!geometryParam && !materialParam && !effectParam && !profileParam) return;

		// If there's ANIMATORS, link them.
		if (geometryParam && geometryParam->GetType() == FCDEffectParameter::FLOAT)
		{
			FCDEffectParameterFloat* animatedParam = (FCDEffectParameterFloat*) geometryParam;
			if (materialParam && materialParam->GetType() == FCDEffectParameter::FLOAT)
			{
				FCDEffectParameterFloat* floatParam = (FCDEffectParameterFloat*) materialParam;
				animatedParam->SetValue(floatParam->GetValue());
			}
			else if (effectParam && effectParam->GetType() == FCDEffectParameter::FLOAT)
			{
				FCDEffectParameterFloat* floatParam = (FCDEffectParameterFloat*) effectParam;
				animatedParam->SetValue(floatParam->GetValue());
			}
			else if (profileParam && profileParam->GetType() == FCDEffectParameter::FLOAT)
			{
				FCDEffectParameterFloat* floatParam = (FCDEffectParameterFloat*) profileParam;
				animatedParam->SetValue(floatParam->GetValue());
			}
		}
	}

	void LinkAnimatedParamCommonVector(const fm::string& semantic, FCDEffectParameterColor4* param, FCDGeometryInstance* geometry, FCDMaterial* material, FCDEffect* effect, FCDEffectProfile* profile)
	{
		//Change this to account for the animated params in geometryInstance list.
		const fm::string& reference = param->GetReference();
		if (reference.empty()) return;
		FCDEffectParameter* geometryParam = FindEffectParameterBySemantic(geometry, semantic);
		FCDEffectParameter* materialParam = FindEffectParameterByReference(material, reference, true);
		FCDEffectParameter* effectParam = FindEffectParameterByReference(effect, reference, true);
		FCDEffectParameter* profileParam = FindEffectParameterByReference(profile, reference, false);
		if (!geometryParam && !materialParam && !effectParam && !profileParam) return;
		if (geometryParam && geometryParam->GetType() == FCDEffectParameter::FLOAT3)
		{
			FCDEffectParameterFloat3* animatedParam = (FCDEffectParameterFloat3*) geometryParam;
			if (materialParam && materialParam->GetType() == FCDEffectParameter::FLOAT3)
			{
				FCDEffectParameterFloat3* float3Param = (FCDEffectParameterFloat3*) materialParam;
				animatedParam->SetValue(float3Param->GetValue());
				return;
			}
			else if (materialParam && materialParam->GetType() == FCDEffectParameter::VECTOR)
			{
				FCDEffectParameterColor4* vectorParam = (FCDEffectParameterColor4*) materialParam;
				animatedParam->SetValue(FMVector3((const FMVector4&) vectorParam->GetValue()));
				return;
			}
			else if (effectParam && effectParam->GetType() == FCDEffectParameter::FLOAT3)
			{
				FCDEffectParameterFloat3* float3Param = (FCDEffectParameterFloat3*) effectParam;
				animatedParam->SetValue(float3Param->GetValue());
				return;
			}
			else if (effectParam && effectParam->GetType() == FCDEffectParameter::VECTOR)
			{
				FCDEffectParameterColor4* vectorParam = (FCDEffectParameterColor4*) effectParam;
				animatedParam->SetValue(FMVector3((const FMVector4&) vectorParam->GetValue()));
				return;
			}
			else if (profileParam && profileParam->GetType() == FCDEffectParameter::FLOAT3)
			{
				FCDEffectParameterFloat3* float3Param = (FCDEffectParameterFloat3*) profileParam;
				animatedParam->SetValue(float3Param->GetValue());
				return;
			}
			else if (profileParam && profileParam->GetType() == FCDEffectParameter::VECTOR)
			{
				FCDEffectParameterColor4* vectorParam = (FCDEffectParameterColor4*) profileParam;
				animatedParam->SetValue(FMVector3((const FMVector4&) vectorParam->GetValue()));
				return;
			}
		}
		else if (geometryParam && geometryParam->GetType() == FCDEffectParameter::VECTOR)
		{
			FCDEffectParameterColor4* animatedParam = (FCDEffectParameterColor4*) geometryParam;
			if (materialParam && materialParam->GetType() == FCDEffectParameter::FLOAT3)
			{
				FCDEffectParameterFloat3* float3Param = (FCDEffectParameterFloat3*) materialParam;
				animatedParam->SetValue(FMVector4(float3Param->GetValue(), 1.0f));
				return;
			}
			else if (materialParam && materialParam->GetType() == FCDEffectParameter::VECTOR)
			{
				FCDEffectParameterColor4* vectorParam = (FCDEffectParameterColor4*) materialParam;
				animatedParam->SetValue(vectorParam->GetValue());
				return;
			}
			else if (effectParam && effectParam->GetType() == FCDEffectParameter::FLOAT3)
			{
				FCDEffectParameterFloat3* float3Param = (FCDEffectParameterFloat3*) effectParam;
				animatedParam->SetValue(FMVector4(float3Param->GetValue(), 1.0f));
				return;
			}
			else if (effectParam && effectParam->GetType() == FCDEffectParameter::VECTOR)
			{
				FCDEffectParameterColor4* vectorParam = (FCDEffectParameterColor4*) effectParam;
				animatedParam->SetValue(vectorParam->GetValue());
				return;
			}
			else if (profileParam && profileParam->GetType() == FCDEffectParameter::FLOAT3)
			{
				FCDEffectParameterFloat3* float3Param = (FCDEffectParameterFloat3*) profileParam;
				animatedParam->SetValue(FMVector4(float3Param->GetValue(), 1.0f));
				return;
			}
			else if (profileParam && profileParam->GetType() == FCDEffectParameter::VECTOR)
			{
				FCDEffectParameterColor4* vectorParam = (FCDEffectParameterColor4*) profileParam;
				animatedParam->SetValue(vectorParam->GetValue());
				return;
			}
		}
	}

	FCDParameterAnimatableColor4* GetAnimatedColor(FCDGeometryInstance* geometryInstance, FCDMaterial* material, const fm::string& semantic, bool* isFloat3)
	{
		//Find the different classes that are needed.
		//FCDMaterial* material = materialInstance->GetMaterial();
		if (material == nullptr) return nullptr;
		FCDEffect* effect = material->GetEffect();
		if (effect == nullptr) return nullptr;
		FCDEffectProfile* effectProfile = effect->FindProfile(FUDaeProfileType::COMMON);
		if (effectProfile == nullptr) return nullptr;
		FCDEffectStandard* effectStandard = (FCDEffectStandard*) effectProfile;
		bool isFloat = true;

		//Find out if the parameter is animated
		FCDEffectParameter* effectStandardParam = effectStandard->GetParam(semantic, &isFloat);
		if (effectStandardParam == nullptr) return nullptr;
		const fm::string& reference = effectStandardParam->GetReference();
		if (reference.empty())
		{
			if (isFloat) return nullptr;
			else return &(((FCDEffectParameterColor4*)effectStandardParam)->GetValue());
		}
		FCDEffectParameter* geometryParam = geometryInstance != nullptr ? FindEffectParameterBySemantic(geometryInstance, semantic) : nullptr;
		FCDEffectParameter* materialParam = FindEffectParameterByReference(material, reference, true);
		FCDEffectParameter* effectParam = FindEffectParameterByReference(effect, reference, true);
		FCDEffectParameter* effectProfileParam = FindEffectParameterByReference(effectProfile, reference, false);

		//Do the figuring out .. ;)
		if (isFloat) return nullptr;
		else
		{
			if (geometryParam)
			{
				if (geometryParam->GetType() == FCDEffectParameter::FLOAT3)
				{
					*isFloat3 = true;
					FCDEffectParameterFloat3* tempParam = (FCDEffectParameterFloat3*) geometryParam;
					return (FCDParameterAnimatableColor4*) &tempParam->GetValue();
				}
				else if (geometryParam->GetType() == FCDEffectParameter::VECTOR)
				{
					*isFloat3 = false;
					return &((FCDEffectParameterColor4*)geometryParam)->GetValue();
				}
				else return nullptr;
			}
			//Could call GetDefaultColor from here on out...
			else if (materialParam)
			{
				if (materialParam->GetType() == FCDEffectParameter::FLOAT3)
				{
					*isFloat3 = true;
					FCDEffectParameterFloat3* tempParam = (FCDEffectParameterFloat3*) materialParam;
					return (FCDParameterAnimatableColor4*) &tempParam->GetValue();
				}
				else if (materialParam->GetType() == FCDEffectParameter::VECTOR)
				{
					*isFloat3 = false;
					return &((FCDEffectParameterColor4*)materialParam)->GetValue();
				}
				else return nullptr;
			}
			else if (effectParam)
			{
				if (effectParam->GetType() == FCDEffectParameter::FLOAT3)
				{
					*isFloat3 = true;
					FCDEffectParameterFloat3* tempParam = (FCDEffectParameterFloat3*) effectParam;
					return (FCDParameterAnimatableColor4*) &tempParam->GetValue();
				}
				else if (effectParam->GetType() == FCDEffectParameter::VECTOR)
				{
					*isFloat3 = false;
					return &((FCDEffectParameterColor4*)effectParam)->GetValue();
				}
				else return nullptr;
			}
			else if (effectProfileParam)
			{
				if (effectProfileParam->GetType() == FCDEffectParameter::FLOAT3)
				{
					*isFloat3 = true;
					FCDEffectParameterFloat3* tempParam = (FCDEffectParameterFloat3*) effectProfileParam;
					return (FCDParameterAnimatableColor4*) &tempParam->GetValue();
				}
				else if (effectProfileParam->GetType() == FCDEffectParameter::VECTOR)
				{
					*isFloat3 = false;
					return &((FCDEffectParameterColor4*)effectProfileParam)->GetValue();
				}
				else return nullptr;
			}
			else
			{
				*isFloat3 = true;
				return &((FCDEffectParameterColor4*)effectStandardParam)->GetValue();
			}
		}
	}

	FCDParameterAnimatableFloat* GetAnimatedFloat(FCDGeometryInstance* geometryInstance, FCDMaterial* material, const fm::string& semantic)
	{
		//Find the different classes that are needed.
		//FCDMaterial* material = materialInstance->GetMaterial();
		if (material == nullptr) return nullptr;
		FCDEffect* effect = material->GetEffect();
		if (effect == nullptr) return nullptr;
		FCDEffectProfile* effectProfile = effect->FindProfile(FUDaeProfileType::COMMON);
		if (effectProfile == nullptr) return nullptr;
		FCDEffectStandard* effectStandard = (FCDEffectStandard*) effectProfile;
		bool isFloat = true;

		//Find out if the parameter is animated
		FCDEffectParameter* effectStandardParam = effectStandard->GetParam(semantic, &isFloat);
		if (!effectStandardParam) return nullptr;
		const fm::string& reference = effectStandardParam->GetReference();
		if (reference.empty())
		{
			if (isFloat) return &(((FCDEffectParameterFloat*)effectStandardParam)->GetValue());
			else return nullptr;
		}
		FCDEffectParameter* geometryParam = geometryInstance != nullptr ? FindEffectParameterBySemantic(geometryInstance, semantic) : nullptr;
		FCDEffectParameter* materialParam = FindEffectParameterByReference(material, reference, true);
		FCDEffectParameter* effectParam = FindEffectParameterByReference(effect, reference, true);
		FCDEffectParameter* effectProfileParam = FindEffectParameterByReference(effectProfile, reference, false);

		if (isFloat)
		{
			if (geometryParam)
			{
				if (geometryParam->GetType() != FCDEffectParameter::FLOAT) return nullptr;
				else return &((FCDEffectParameterFloat*)geometryParam)->GetValue();
			}
			//From this on out, could call GetDefaultFloat... and return that?
			else if (materialParam)
			{
				if (materialParam->GetType() != FCDEffectParameter::FLOAT) return nullptr;
				else return &((FCDEffectParameterFloat*)materialParam)->GetValue();
			}
			else if (effectParam)
			{
				if (effectParam->GetType() != FCDEffectParameter::FLOAT) return nullptr;
				else return &((FCDEffectParameterFloat*)effectParam)->GetValue();
			}
			else if (effectProfileParam)
			{
				if (effectProfileParam->GetType() != FCDEffectParameter::FLOAT) return nullptr;
				else return &((FCDEffectParameterFloat*)effectProfileParam)->GetValue();
			}
			else return &((FCDEffectParameterFloat*)effectStandardParam)->GetValue();
		}
		else return nullptr;
	}

	FMVector4* GetDefaultColor(FCDMaterial* material, const fm::string& semantic, bool* isFloat3)
	{
		//Find the different classes that are needed.
		FCDEffect* effect = material->GetEffect();
		FCDEffectProfile* effectProfile = effect->FindProfile(FUDaeProfileType::COMMON);
		FCDEffectStandard* effectStandard = (FCDEffectStandard*) effectProfile;
		bool isFloat = true;

		//Find out if the parameter is animated
		FCDEffectParameter* effectStandardParam = effectStandard->GetParam(semantic, &isFloat);
		if (!effectStandardParam) return nullptr;
		const fm::string& reference = effectStandardParam->GetReference();
		if (reference.empty())
		{
			if (isFloat) return nullptr;
			else return &(FMVector4&)(((FCDEffectParameterColor4*)effectStandardParam)->GetValue());
		}
		FCDEffectParameter* materialParam = FindEffectParameterByReference(material, reference, true);
		FCDEffectParameter* effectParam = FindEffectParameterByReference(effect, reference, true);
		FCDEffectParameter* effectProfileParam = FindEffectParameterByReference(effectProfile, reference, false);

		//Do the figuring out .. ;)
		if (isFloat) return nullptr;
		else
		{
			// Don't look at the geometry instance parameters: these are only used for animations!
			if (materialParam)
			{
				if (materialParam->GetType() == FCDEffectParameter::FLOAT3)
				{
					*isFloat3 = true;
					FCDEffectParameterFloat3* tempParam = (FCDEffectParameterFloat3*) materialParam;
					return (FMVector4*) &(FMVector3&) tempParam->GetValue();
				}
				else if (materialParam->GetType() == FCDEffectParameter::VECTOR)
				{
					*isFloat3 = false;
					return &(FMVector4&) ((FCDEffectParameterColor4*)materialParam)->GetValue();
				}
				else return nullptr;
			}
			else if (effectParam)
			{
				if (effectParam->GetType() == FCDEffectParameter::FLOAT3)
				{
					*isFloat3 = true;
					FCDEffectParameterFloat3* tempParam = (FCDEffectParameterFloat3*) effectParam;
					return (FMVector4*) &(FMVector3&) tempParam->GetValue();
				}
				else if (effectParam->GetType() == FCDEffectParameter::VECTOR)
				{
					*isFloat3 = false;
					return &(FMVector4&) ((FCDEffectParameterColor4*)effectParam)->GetValue();
				}
				else return nullptr;
			}
			else if (effectProfileParam)
			{
				if (effectProfileParam->GetType() == FCDEffectParameter::FLOAT3)
				{
					*isFloat3 = true;
					FCDEffectParameterFloat3* tempParam = (FCDEffectParameterFloat3*) effectProfileParam;
					return (FMVector4*) &(FMVector3&) tempParam->GetValue();
				}
				else if (effectProfileParam->GetType() == FCDEffectParameter::VECTOR)
				{
					*isFloat3 = false;
					return &(FMVector4&) ((FCDEffectParameterColor4*)effectProfileParam)->GetValue();
				}
				else return nullptr;
			}
			else
			{
				*isFloat3 = true;
				return &(FMVector4&) ((FCDEffectParameterColor4*)effectStandardParam)->GetValue();
			}
		}
	}

	float* GetDefaultFloat(FCDMaterial* material, const fm::string& semantic)
	{
		//Find the different classes that are needed.
		FCDEffect* effect = material->GetEffect();
		FCDEffectProfile* effectProfile = effect->FindProfile(FUDaeProfileType::COMMON);
		FCDEffectStandard* effectStandard = (FCDEffectStandard*) effectProfile;
		bool isFloat = true;

		//Find out if the parameter is animated
		FCDEffectParameter* effectStandardParam = effectStandard->GetParam(semantic, &isFloat);
		if (!effectStandardParam) return nullptr;
		const fm::string& reference = effectStandardParam->GetReference();
		if (reference.empty())
		{
			if (isFloat) return &(float&) ((FCDEffectParameterFloat*)effectStandardParam)->GetValue();
			else return nullptr;
		}
		FCDEffectParameter* materialParam = FindEffectParameterByReference(material, reference, true);
		FCDEffectParameter* effectParam = FindEffectParameterByReference(effect, reference, true);
		FCDEffectParameter* effectProfileParam = FindEffectParameterByReference(effectProfile, reference, false);

		if (isFloat)
		{
			if (materialParam)
			{
				if (materialParam->GetType() != FCDEffectParameter::FLOAT) return nullptr;
				else return &(float&) ((FCDEffectParameterFloat*)materialParam)->GetValue();
			}
			else if (effectParam)
			{
				if (effectParam->GetType() != FCDEffectParameter::FLOAT) return nullptr;
				else return &(float&) ((FCDEffectParameterFloat*)effectParam)->GetValue();
			}
			else if (effectProfileParam)
			{
				if (effectProfileParam->GetType() != FCDEffectParameter::FLOAT) return nullptr;
				else return &(float&) ((FCDEffectParameterFloat*)effectProfileParam)->GetValue();
			}
			else return &(float&) ((FCDEffectParameterFloat*)effectStandardParam)->GetValue();
		}
		else return nullptr;
	}

	//
	// FindEffectParameterBySemantic
	//

	const FCDEffectParameter* FindEffectParameterBySemantic(const FCDGeometryInstance* geometryInstance, const char* semantic)
	{
		if (geometryInstance == nullptr || semantic == nullptr || *semantic == 0) return nullptr;
		size_t count = geometryInstance->GetEffectParameterCount();
		for (size_t p = 0; p < count; ++p)
		{
			const FCDEffectParameter* effectParameter = geometryInstance->GetEffectParameter(p);
			if (IsEquivalent(effectParameter->GetSemantic(), semantic)) return effectParameter;
		}
		return nullptr;
	}

	const FCDEffectParameter* FindEffectParameterBySemantic(const FCDMaterialInstance* materialInstance, const char* semantic, bool localOnly)
	{
		if (materialInstance == nullptr || semantic == nullptr || *semantic == 0) return nullptr;
		const FCDGeometryInstance* geometryInstance = (FCDGeometryInstance*) materialInstance->GetParent();
		size_t count = geometryInstance->GetEffectParameterCount();
		for (size_t p = 0; p < count; ++p)
		{
			const FCDEffectParameter* effectParameter = geometryInstance->GetEffectParameter(p);
			if (IsEquivalent(effectParameter->GetSemantic(), semantic)) return effectParameter;
		}
		return !localOnly ? FindEffectParameterBySemantic(materialInstance->GetMaterial(), semantic) : nullptr;
	}

	const FCDEffectParameter* FindEffectParameterBySemantic(const FCDMaterial* material, const char* semantic, bool localOnly)
	{
		if (material == nullptr || semantic == nullptr || *semantic == 0) return nullptr;
		size_t count = material->GetEffectParameterCount();
		for (size_t p = 0; p < count; ++p)
		{
			const FCDEffectParameter* effectParameter = material->GetEffectParameter(p);
			if (IsEquivalent(effectParameter->GetSemantic(), semantic)) return effectParameter;
		}
		return !localOnly ? FindEffectParameterBySemantic(material->GetEffect(), semantic) : nullptr;
	}

	const FCDEffectParameter* FindEffectParameterBySemantic(const FCDEffect* effect, const char* semantic, bool localOnly)
	{
		if (effect == nullptr || semantic == nullptr || *semantic == 0) return nullptr;
		size_t count = effect->GetEffectParameterCount();
		for (size_t p = 0; p < count; ++p)
		{
			const FCDEffectParameter* effectParameter = effect->GetEffectParameter(p);
			if (IsEquivalent(effectParameter->GetSemantic(), semantic)) return effectParameter;
		}
		if (!localOnly)
		{
			size_t profileCount = effect->GetProfileCount();
			for (size_t p = 0; p < profileCount; ++p)
			{
				const FCDEffectParameter* effectParameter = FindEffectParameterBySemantic(effect->GetProfile(p), semantic);
				if (effectParameter != nullptr) return effectParameter;
			}
		}
		return nullptr;
	}

	const FCDEffectParameter* FindEffectParameterBySemantic(const FCDEffectProfile* profile, const char* semantic, bool localOnly)
	{
		// Look within the local parameters.
		if (profile == nullptr || semantic == nullptr || *semantic == 0) return nullptr;
		size_t count = profile->GetEffectParameterCount();
		for (size_t p = 0; p < count; ++p)
		{
			const FCDEffectParameter* effectParameter = profile->GetEffectParameter(p);
			if (IsEquivalent(effectParameter->GetSemantic(), semantic)) return effectParameter;
		}

		if (profile->HasType(FCDEffectProfileFX::GetClassType()) && !localOnly)
		{
			// Look within the <technique> parameters.
			FCDEffectProfileFX* fx = (FCDEffectProfileFX*) profile;
			size_t techniqueCount = fx->GetTechniqueCount();
			for (size_t t = 0; t < techniqueCount; ++t)
			{
				const FCDEffectParameter* effectParameter = FindEffectParameterBySemantic(fx->GetTechnique(t), semantic);
				if (effectParameter != nullptr) return effectParameter;
			}
		}
		else if (profile->HasType(FCDEffectStandard::GetClassType()))
		{
			// Look within the textures which have their own parameters.
			FCDEffectStandard* std = (FCDEffectStandard*) profile;
			for (uint32 i = 0; i < FUDaeTextureChannel::COUNT; ++i)
			{
				size_t bucketSize = std->GetTextureCount(i);
				for (size_t t = 0; t < bucketSize; ++t)
				{
					FCDTexture* texture = std->GetTexture(i, t);
					if (IsEquivalent(texture->GetSet()->GetSemantic(), semantic)) return texture->GetSet();
				}
			}
		}
		return nullptr;
	}

	const FCDEffectParameter* FindEffectParameterBySemantic(const FCDEffectTechnique* technique, const char* semantic, bool UNUSED(localOnly))
	{
		if (technique == nullptr || semantic == nullptr || *semantic == 0) return nullptr;
		size_t count = technique->GetEffectParameterCount();
		for (size_t p = 0; p < count; ++p)
		{
			const FCDEffectParameter* effectParameter = technique->GetEffectParameter(p);
			if (IsEquivalent(effectParameter->GetSemantic(), semantic)) return effectParameter;
		}
		return nullptr;
	}

	//
	// FindEffectParameterByReference
	//

	const FCDEffectParameter* FindEffectParameterByReference(const FCDGeometryInstance* geometryInstance, const char* reference)
	{
		if (geometryInstance == nullptr || reference == nullptr || *reference == 0) return nullptr;
		size_t count = geometryInstance->GetEffectParameterCount();
		for (size_t p = 0; p < count; ++p)
		{
			const FCDEffectParameter* effectParameter = geometryInstance->GetEffectParameter(p);
			if (IsEquivalent(effectParameter->GetReference(), reference)) return effectParameter;
		}
		return nullptr;
	}

	const FCDEffectParameter* FindEffectParameterByReference(const FCDMaterialInstance* materialInstance, const char* reference, bool localOnly)
	{
		if (materialInstance == nullptr || reference == nullptr || *reference == 0) return nullptr;
		const FCDGeometryInstance* geometryInstance = (FCDGeometryInstance*) materialInstance->GetParent();
		size_t count = geometryInstance->GetEffectParameterCount();
		for (size_t p = 0; p < count; ++p)
		{
			const FCDEffectParameter* effectParameter = geometryInstance->GetEffectParameter(p);
			if (IsEquivalent(effectParameter->GetReference(), reference)) return effectParameter;
		}
		return !localOnly ? FindEffectParameterByReference(materialInstance->GetMaterial(), reference) : nullptr;
	}

	const FCDEffectParameter* FindEffectParameterByReference(const FCDMaterial* material, const char* reference, bool localOnly)
	{
		if (material == nullptr || reference == nullptr || *reference == 0) return nullptr;
		size_t count = material->GetEffectParameterCount();
		for (size_t p = 0; p < count; ++p)
		{
			const FCDEffectParameter* effectParameter = material->GetEffectParameter(p);
			if (IsEquivalent(effectParameter->GetReference(), reference)) return effectParameter;
		}
		return !localOnly ? FindEffectParameterByReference(material->GetEffect(), reference) : nullptr;
	}

	const FCDEffectParameter* FindEffectParameterByReference(const FCDEffect* effect, const char* reference, bool localOnly)
	{
		if (effect == nullptr || reference == nullptr || *reference == 0) return nullptr;
		size_t count = effect->GetEffectParameterCount();
		for (size_t p = 0; p < count; ++p)
		{
			const FCDEffectParameter* effectParameter = effect->GetEffectParameter(p);
			if (IsEquivalent(effectParameter->GetReference(), reference)) return effectParameter;
		}
		if (!localOnly)
		{
			size_t profileCount = effect->GetProfileCount();
			for (size_t p = 0; p < profileCount; ++p)
			{
				const FCDEffectParameter* effectParameter = FindEffectParameterByReference(effect->GetProfile(p), reference);
				if (effectParameter != nullptr) return effectParameter;
			}
		}
		return nullptr;
	}

	const FCDEffectParameter* FindEffectParameterByReference(const FCDEffectProfile* profile, const char* reference, bool localOnly)
	{
		// Look within the local parameters.
		if (profile == nullptr || reference == nullptr || *reference == 0) return nullptr;
		size_t count = profile->GetEffectParameterCount();
		for (size_t p = 0; p < count; ++p)
		{
			const FCDEffectParameter* effectParameter = profile->GetEffectParameter(p);
			if (IsEquivalent(effectParameter->GetReference(), reference)) return effectParameter;
		}

		if (profile->HasType(FCDEffectProfileFX::GetClassType()) && !localOnly)
		{
			// Look within the <technique> parameters.
			FCDEffectProfileFX* fx = (FCDEffectProfileFX*) profile;
			size_t techniqueCount = fx->GetTechniqueCount();
			for (size_t t = 0; t < techniqueCount; ++t)
			{
				const FCDEffectParameter* effectParameter = FindEffectParameterByReference(fx->GetTechnique(t), reference);
				if (effectParameter != nullptr) return effectParameter;
			}
		}
		else if (profile->HasType(FCDEffectStandard::GetClassType()))
		{
			// Look within the textures which have their own parameters.
			FCDEffectStandard* std = (FCDEffectStandard*) profile;
			for (uint32 i = 0; i < FUDaeTextureChannel::COUNT; ++i)
			{
				size_t bucketSize = std->GetTextureCount(i);
				for (size_t t = 0; t < bucketSize; ++t)
				{
					FCDTexture* texture = std->GetTexture(i, t);
					if (IsEquivalent(texture->GetSet()->GetReference(), reference)) return texture->GetSet();
				}
			}
		}
		return nullptr;
	}

	const FCDEffectParameter* FindEffectParameterByReference(const FCDEffectTechnique* technique, const char* reference, bool UNUSED(localOnly))
	{
		if (technique == nullptr || reference == nullptr || *reference == 0) return nullptr;
		size_t count = technique->GetEffectParameterCount();
		for (size_t p = 0; p < count; ++p)
		{
			const FCDEffectParameter* effectParameter = technique->GetEffectParameter(p);
			if (IsEquivalent(effectParameter->GetReference(), reference)) return effectParameter;
		}
		return nullptr;
	}

	//
	// FindEffectParametersBySemantic
	//

	void FindEffectParametersBySemantic(const FCDMaterialInstance* materialInstance, const char* semantic, FCDEffectParameterList& parameters, bool localOnly)
	{
		if (materialInstance == nullptr || semantic == nullptr || *semantic == 0) return;
		const FCDGeometryInstance* geometryInstance = (FCDGeometryInstance*) materialInstance->GetParent();
		size_t count = geometryInstance->GetEffectParameterCount();
		for (size_t p = 0; p < count; ++p)
		{
			const FCDEffectParameter* effectParameter = geometryInstance->GetEffectParameter(p);
			if (IsEquivalent(effectParameter->GetSemantic(), semantic)) parameters.push_back(effectParameter);
		}
		if (!localOnly)
		{
			FindEffectParametersBySemantic(materialInstance->GetMaterial(), semantic, parameters);
		}
	}

	void FindEffectParametersBySemantic(const FCDMaterial* material, const char* semantic, FCDEffectParameterList& parameters, bool localOnly)
	{
		if (material == nullptr || semantic == nullptr || *semantic == 0) return;
		size_t count = material->GetEffectParameterCount();
		for (size_t p = 0; p < count; ++p)
		{
			const FCDEffectParameter* effectParameter = material->GetEffectParameter(p);
			if (IsEquivalent(effectParameter->GetSemantic(), semantic)) parameters.push_back(effectParameter);
		}
		if (!localOnly)
		{
			FindEffectParametersBySemantic(material->GetEffect(), semantic, parameters);
		}
	}

	void FindEffectParametersBySemantic(const FCDEffect* effect, const char* semantic, FCDEffectParameterList& parameters, bool localOnly)
	{
		if (effect == nullptr || semantic == nullptr || *semantic == 0) return;
		size_t count = effect->GetEffectParameterCount();
		for (size_t p = 0; p < count; ++p)
		{
			const FCDEffectParameter* effectParameter = effect->GetEffectParameter(p);
			if (IsEquivalent(effectParameter->GetSemantic(), semantic)) parameters.push_back(effectParameter);
		}
		if (!localOnly)
		{
			size_t profileCount = effect->GetProfileCount();
			for (size_t p = 0; p < profileCount; ++p)
			{
				FindEffectParametersBySemantic(effect->GetProfile(p), semantic, parameters);
			}
		}
	}

	void FindEffectParametersBySemantic(const FCDEffectProfile* profile, const char* semantic, FCDEffectParameterList& parameters, bool localOnly)
	{
		// Look within the local parameters.
		if (profile == nullptr || semantic == nullptr || *semantic == 0) return;
		size_t count = profile->GetEffectParameterCount();
		for (size_t p = 0; p < count; ++p)
		{
			const FCDEffectParameter* effectParameter = profile->GetEffectParameter(p);
			if (IsEquivalent(effectParameter->GetSemantic(), semantic)) parameters.push_back(effectParameter);
		}

		if (profile->HasType(FCDEffectProfileFX::GetClassType()) && !localOnly)
		{
			// Look within the <technique> parameters.
			FCDEffectProfileFX* fx = (FCDEffectProfileFX*) profile;
			size_t techniqueCount = fx->GetTechniqueCount();
			for (size_t t = 0; t < techniqueCount; ++t)
			{
				FindEffectParametersBySemantic(fx->GetTechnique(t), semantic, parameters);
			}
		}
		else if (profile->HasType(FCDEffectStandard::GetClassType()))
		{
			// Look within the textures which have their own parameters.
			FCDEffectStandard* std = (FCDEffectStandard*) profile;
			for (uint32 i = 0; i < FUDaeTextureChannel::COUNT; ++i)
			{
				size_t bucketSize = std->GetTextureCount(i);
				for (size_t t = 0; t < bucketSize; ++t)
				{
					FCDTexture* texture = std->GetTexture(i, t);
					if (IsEquivalent(texture->GetSet()->GetSemantic(), semantic)) parameters.push_back(texture->GetSet());
				}
			}
		}
	}

	void FindEffectParametersBySemantic(const FCDEffectTechnique* technique, const char* semantic, FCDEffectParameterList& parameters, bool UNUSED(localOnly))
	{
		if (technique == nullptr || semantic == nullptr || *semantic == 0) return;
		size_t count = technique->GetEffectParameterCount();
		for (size_t p = 0; p < count; ++p)
		{
			const FCDEffectParameter* effectParameter = technique->GetEffectParameter(p);
			if (IsEquivalent(effectParameter->GetSemantic(), semantic)) parameters.push_back(effectParameter);
		}
	}

	//
	// FindEffectParametersByReference
	//

	void FindEffectParametersByReference(const FCDMaterialInstance* materialInstance, const char* reference, FCDEffectParameterList& parameters, bool localOnly)
	{
		if (materialInstance == nullptr || reference == nullptr || *reference == 0) return;
		const FCDGeometryInstance* geometryInstance = (FCDGeometryInstance*) materialInstance->GetParent();
		size_t count = geometryInstance->GetEffectParameterCount();
		for (size_t p = 0; p < count; ++p)
		{
			const FCDEffectParameter* effectParameter = geometryInstance->GetEffectParameter(p);
			if (IsEquivalent(effectParameter->GetReference(), reference)) parameters.push_back(effectParameter);
		}
		if (!localOnly)
		{
			FindEffectParametersByReference(materialInstance->GetMaterial(), reference, parameters);
		}
	}

	void FindEffectParametersByReference(const FCDMaterial* material, const char* reference, FCDEffectParameterList& parameters, bool localOnly)
	{
		if (material == nullptr || reference == nullptr || *reference == 0) return;
		size_t count = material->GetEffectParameterCount();
		for (size_t p = 0; p < count; ++p)
		{
			const FCDEffectParameter* effectParameter = material->GetEffectParameter(p);
			if (IsEquivalent(effectParameter->GetReference(), reference)) parameters.push_back(effectParameter);
		}
		if (!localOnly)
		{
			FindEffectParametersByReference(material->GetEffect(), reference, parameters);
		}
	}

	void FindEffectParametersByReference(const FCDEffect* effect, const char* reference, FCDEffectParameterList& parameters, bool localOnly)
	{
		if (effect == nullptr || reference == nullptr || *reference == 0) return;
		size_t count = effect->GetEffectParameterCount();
		for (size_t p = 0; p < count; ++p)
		{
			const FCDEffectParameter* effectParameter = effect->GetEffectParameter(p);
			if (IsEquivalent(effectParameter->GetReference(), reference)) parameters.push_back(effectParameter);
		}
		if (!localOnly)
		{
			size_t profileCount = effect->GetProfileCount();
			for (size_t p = 0; p < profileCount; ++p)
			{
				FindEffectParametersByReference(effect->GetProfile(p), reference, parameters);
			}
		}
	}

	void FindEffectParametersByReference(const FCDEffectProfile* profile, const char* reference, FCDEffectParameterList& parameters, bool localOnly)
	{
		// Look within the local parameters.
		if (profile == nullptr || reference == nullptr || *reference == 0) return;
		size_t count = profile->GetEffectParameterCount();
		for (size_t p = 0; p < count; ++p)
		{
			const FCDEffectParameter* effectParameter = profile->GetEffectParameter(p);
			if (IsEquivalent(effectParameter->GetReference(), reference)) parameters.push_back(effectParameter);
		}

		if (profile->HasType(FCDEffectProfileFX::GetClassType()) && !localOnly)
		{
			// Look within the <technique> parameters.
			FCDEffectProfileFX* fx = (FCDEffectProfileFX*) profile;
			size_t techniqueCount = fx->GetTechniqueCount();
			for (size_t t = 0; t < techniqueCount; ++t)
			{
				FindEffectParametersByReference(fx->GetTechnique(t), reference, parameters);
			}
		}
		else if (profile->HasType(FCDEffectStandard::GetClassType()))
		{
			// Look within the textures which have their own parameters.
			FCDEffectStandard* std = (FCDEffectStandard*) profile;
			for (uint32 i = 0; i < FUDaeTextureChannel::COUNT; ++i)
			{
				size_t bucketSize = std->GetTextureCount(i);
				for (size_t t = 0; t < bucketSize; ++t)
				{
					FCDTexture* texture = std->GetTexture(i, t);
					if (IsEquivalent(texture->GetSet()->GetReference(), reference)) parameters.push_back(texture->GetSet());
				}
			}
		}
	}

	void FindEffectParametersByReference(const FCDEffectTechnique* technique, const char* reference, FCDEffectParameterList& parameters, bool UNUSED(localOnly))
	{
		if (technique == nullptr || reference == nullptr || *reference == 0) return;
		size_t count = technique->GetEffectParameterCount();
		for (size_t p = 0; p < count; ++p)
		{
			const FCDEffectParameter* effectParameter = technique->GetEffectParameter(p);
			if (IsEquivalent(effectParameter->GetReference(), reference)) parameters.push_back(effectParameter);
		}
	}
};
