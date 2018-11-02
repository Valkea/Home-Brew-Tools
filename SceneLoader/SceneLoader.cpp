#include "SceneLoader.h"

namespace hbt
{
//*************************************************************************************************/
///// SceneLoader /////////////////////////////////////////////////////////////////////////////////

SceneLoader::SceneLoader()
{
	std::cout << "Hello From Scene Loader !! " << std::endl;
}


SceneLoader::~SceneLoader()
{
	std::cout << "Goodbye From Scene Loader !! " << std::endl;
}

void SceneLoader::open(const std::string _sceneFile, Ogre::SceneManager* _localSceneMgr, bool _setEntities, bool _setLights, bool _setCamera, float _offsetX, float _offsetY, float _offsetZ)
{
	std::string sceneName = _sceneFile.substr(0, _sceneFile.length()-6);
	Ogre::SceneNode* sceneNode = _localSceneMgr->getRootSceneNode()->createChildSceneNode("node_"+sceneName);

	m_offsetX = _offsetX;
	m_offsetY = _offsetY;
	m_offsetZ = _offsetZ;

	#ifdef __APPLE__
	      std::string sceneFile = hbt::exe_path()+"../Resources/media/models/"+_sceneFile;
	#else
	      std::string sceneFile = hbt::exe_path()+"media/models/"+_sceneFile;
	#endif

	      std::cout << "sceneFile: " << sceneFile << std::endl;

	TiXmlDocument sceneDoc( sceneFile.c_str() );
	sceneDoc.LoadFile();
	if ( sceneDoc.Error() && sceneDoc.ErrorId() == TiXmlBase::TIXML_ERROR_OPENING_FILE ) {
		std::cout << "WARNING: Scene file '" << _sceneFile << "' not found." << std::endl;
	}
	else
	{
		TiXmlHandle docH( &sceneDoc );
		TiXmlElement* element = docH.FirstChildElement( "scene" ).FirstChildElement( "nodes" ).Element();

		std::cout << "Debug:" << element->Value() << std::endl;

		element = element->FirstChildElement();
		for( element; element; element=element->NextSiblingElement())
		{

			if( _setEntities && element->FirstChildElement("entity") )
			{
				std::cout << "Entity" << std::endl;
				createEntity( sceneName, _localSceneMgr, sceneNode, element );
			}
			else if( _setLights && element->FirstChildElement("light") )
			{
				std::cout << "Light" << std::endl;
				createLight( sceneName, _localSceneMgr, sceneNode, element );
			}
			else if( _setCamera && element->FirstChildElement("camera") )
			{
				TiXmlElement* entElement = element->FirstChildElement("camera");

				std::string name = element->Attribute("name");
				std::cout << "camera name:" << name << std::endl;
				std::string type = entElement->Attribute( "projectionType" );
				std::cout << "camera projectionType:" << type << std::endl;
			}
		}

		std::cout << "Close loop" << std::endl;
	}
}



Ogre::SceneNode* SceneLoader::createNode( const std::string _name, Ogre::SceneNode* _sceneNode, TiXmlElement* _element, bool _setPos, bool _setRot, bool _setScale )
{
	Ogre::SceneNode* node = _sceneNode->createChildSceneNode( _name );

	if( _setPos )
	{
		node->setPosition(
				fromString<float>(_element->FirstChildElement("position")->Attribute("x"))+m_offsetX,
				fromString<float>(_element->FirstChildElement("position")->Attribute("y"))+m_offsetY,
				fromString<float>(_element->FirstChildElement("position")->Attribute("z"))+m_offsetZ);
	}

	if( _setRot )
	{
		node->setOrientation(
				fromString<float>(_element->FirstChildElement("rotation")->Attribute("qw")),
				fromString<float>(_element->FirstChildElement("rotation")->Attribute("qx")),
				fromString<float>(_element->FirstChildElement("rotation")->Attribute("qy")),
				fromString<float>(_element->FirstChildElement("rotation")->Attribute("qz")));
	}

	if( _setScale )
	{
		node->setScale(
				fromString<float>(_element->FirstChildElement("scale")->Attribute("x")),
				fromString<float>(_element->FirstChildElement("scale")->Attribute("y")),
				fromString<float>(_element->FirstChildElement("scale")->Attribute("z")));
	}

	return node;
}


void SceneLoader::createEntity( const std::string _name, Ogre::SceneManager* _localSceneMgr, Ogre::SceneNode* _sceneNode, TiXmlElement* _element )
{
	std::string elementName = std::string(_element->Attribute("name"))+"."+_name;

	Ogre::SceneNode* node = createNode( "node_"+elementName, _sceneNode, _element, true, true, true );

	Ogre::Entity* ent = _localSceneMgr->createEntity( "entity_"+elementName, _element->FirstChildElement("entity")->Attribute("meshFile") );
	node->attachObject(ent);

	//ent->setCastShadows(true);
	//ent->setMaterialName("Wood01");
}


void SceneLoader::createLight( const std::string _name, Ogre::SceneManager* _localSceneMgr, Ogre::SceneNode* _sceneNode, TiXmlElement* _element )
{
	TiXmlElement* lightElement = _element->FirstChildElement("light");
	std::string type = lightElement->Attribute( "type" );

	/*if( type == "directional" )	;//return;
	else if( type == "spot" )	;//return;
	else						;//return;*/

	//m_lightNumber++; //ToDo deactivated but not to delete until the shadows are OK

	std::string elementName = std::string(_element->Attribute("name"))+"."+_name;

	Ogre::SceneNode* node = createNode( "node_"+elementName, _sceneNode, _element, true, false, true );

	Ogre::Light* lLight = _localSceneMgr->createLight( "light_"+elementName );

	Ogre::BillboardSet* bbs = _localSceneMgr->createBillboardSet(1);
	Ogre::Billboard* bb = bbs->createBillboard(0, 0, 0, Ogre::ColourValue(1,1,1));
	bbs->setMaterialName("Flare02");
	bb->setDimensions(1,1);

	//TiXmlElement* lightElement = _element->FirstChildElement("light");

	//-- Type
	//std::string type = lightElement->Attribute( "type" );
	if( type == "directional" )
	{
		lLight->setType(Ogre::Light::LT_DIRECTIONAL);	// Sun
	}
	else if( type == "spot" )
	{
		lLight->setType(Ogre::Light::LT_SPOTLIGHT);		// Spot
	}
	else
	{
		lLight->setType(Ogre::Light::LT_POINT);			// Point (Hemi, Area)
	}

	//-- Power Scale
	lLight->setPowerScale( fromString<float>(lightElement->Attribute("powerScale")) );

	//-- Cast shadows
	//if( !lightElement->Attribute("shadow") )
	{
	//	lLight->setShadowFarDistance(0.1f);
	//	lLight->setShadowNearClipDistance(0.1f);
	}

	//-- Light Attenuation
	lightElement = lightElement->FirstChildElement("lightAttenuation");
	lLight->setAttenuation(
				fromString<float>(lightElement->Attribute("range")),
				fromString<float>(lightElement->Attribute("constant")),
				fromString<float>(lightElement->Attribute("linear")),
				fromString<float>(lightElement->Attribute("quadratic")));

	if( type == "spot" || type == "directional" )
	{
		//-- Spot Light Direction
		lightElement = lightElement->NextSiblingElement("direction");
		lLight->setDirection(
					fromString<float>(lightElement->Attribute("x")),
					fromString<float>(lightElement->Attribute("y")),
					fromString<float>(lightElement->Attribute("z")));
	}

	if( type == "spot" )
	{
		//-- Spot Light Range
		lightElement = lightElement->NextSiblingElement("spotLightRange");
		lLight->setSpotlightRange(
					Ogre::Radian(fromString<float>(lightElement->Attribute("inner"))),
					Ogre::Radian(fromString<float>(lightElement->Attribute("outer"))),
					fromString<float>(lightElement->Attribute("falloff")) );
	}

	//-- Diffuse Color
	lightElement = lightElement->NextSiblingElement("colourDiffuse");
	lLight->setDiffuseColour(
				fromString<float>(lightElement->Attribute("r")),
				fromString<float>(lightElement->Attribute("g")),
				fromString<float>(lightElement->Attribute("b")));

	bb->setColour( Ogre::ColourValue(
					fromString<float>(lightElement->Attribute("r")),
					fromString<float>(lightElement->Attribute("g")),
					fromString<float>(lightElement->Attribute("b"))) );

	//-- Specular Color
	lightElement = lightElement->NextSiblingElement("colourSpecular");
	lLight->setSpecularColour(
				fromString<float>(lightElement->Attribute("r")),
				fromString<float>(lightElement->Attribute("g")),
				fromString<float>(lightElement->Attribute("b")));


	node->attachObject(lLight);
	node->attachObject(bbs);
}

}
