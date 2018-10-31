// This file is auto-generated, do not edit it by hand
#include <stdexcept>
#include <string>

#include <modbox/graphics/enums.hpp>

#include <irrlicht_wrapper.hpp>
int stringToIrrlichtEnum(const std::string& s)
{
    using namespace irr;
    using namespace core;
    using namespace scene;
    using namespace video;
    using namespace gui;
    using namespace io;
    using namespace quake3;
    if (s == "EAAM_ALPHA_TO_COVERAGE") {
        return static_cast<int>(EAAM_ALPHA_TO_COVERAGE);
    }
    if (s == "EAAM_FULL_BASIC") {
        return static_cast<int>(EAAM_FULL_BASIC);
    }
    if (s == "EAAM_LINE_SMOOTH") {
        return static_cast<int>(EAAM_LINE_SMOOTH);
    }
    if (s == "EAAM_OFF") {
        return static_cast<int>(EAAM_OFF);
    }
    if (s == "EAAM_POINT_SMOOTH") {
        return static_cast<int>(EAAM_POINT_SMOOTH);
    }
    if (s == "EAAM_QUALITY") {
        return static_cast<int>(EAAM_QUALITY);
    }
    if (s == "EAAM_SIMPLE") {
        return static_cast<int>(EAAM_SIMPLE);
    }
    if (s == "EAC_BOX") {
        return static_cast<int>(EAC_BOX);
    }
    if (s == "EAC_FRUSTUM_BOX") {
        return static_cast<int>(EAC_FRUSTUM_BOX);
    }
    if (s == "EAC_FRUSTUM_SPHERE") {
        return static_cast<int>(EAC_FRUSTUM_SPHERE);
    }
    if (s == "EAC_OCC_QUERY") {
        return static_cast<int>(EAC_OCC_QUERY);
    }
    if (s == "EAC_OFF") {
        return static_cast<int>(EAC_OFF);
    }
    if (s == "EAMT_3DS") {
        return static_cast<int>(EAMT_3DS);
    }
    if (s == "EAMT_BSP") {
        return static_cast<int>(EAMT_BSP);
    }
    if (s == "EAMT_CSM") {
        return static_cast<int>(EAMT_CSM);
    }
    if (s == "EAMT_LMTS") {
        return static_cast<int>(EAMT_LMTS);
    }
    if (s == "EAMT_MD2") {
        return static_cast<int>(EAMT_MD2);
    }
    if (s == "EAMT_MD3") {
        return static_cast<int>(EAMT_MD3);
    }
    if (s == "EAMT_MDL_HALFLIFE") {
        return static_cast<int>(EAMT_MDL_HALFLIFE);
    }
    if (s == "EAMT_MY3D") {
        return static_cast<int>(EAMT_MY3D);
    }
    if (s == "EAMT_OBJ") {
        return static_cast<int>(EAMT_OBJ);
    }
    if (s == "EAMT_OCT") {
        return static_cast<int>(EAMT_OCT);
    }
    if (s == "EAMT_SKINNED") {
        return static_cast<int>(EAMT_SKINNED);
    }
    if (s == "EAMT_UNKNOWN") {
        return static_cast<int>(EAMT_UNKNOWN);
    }
    if (s == "EARWF_FOR_EDITOR") {
        return static_cast<int>(EARWF_FOR_EDITOR);
    }
    if (s == "EARWF_FOR_FILE") {
        return static_cast<int>(EARWF_FOR_FILE);
    }
    if (s == "EARWF_USE_RELATIVE_PATHS") {
        return static_cast<int>(EARWF_USE_RELATIVE_PATHS);
    }
    if (s == "EAS_NONE") {
        return static_cast<int>(EAS_NONE);
    }
    if (s == "EAS_TEXTURE") {
        return static_cast<int>(EAS_TEXTURE);
    }
    if (s == "EAS_VERTEX_COLOR") {
        return static_cast<int>(EAS_VERTEX_COLOR);
    }
    if (s == "EAT_BBOX") {
        return static_cast<int>(EAT_BBOX);
    }
    if (s == "EAT_BINARY") {
        return static_cast<int>(EAT_BINARY);
    }
    if (s == "EAT_BOOL") {
        return static_cast<int>(EAT_BOOL);
    }
    if (s == "EAT_COLORF") {
        return static_cast<int>(EAT_COLORF);
    }
    if (s == "EAT_COLOR") {
        return static_cast<int>(EAT_COLOR);
    }
    if (s == "EAT_COUNT") {
        return static_cast<int>(EAT_COUNT);
    }
    if (s == "EAT_DIMENSION2D") {
        return static_cast<int>(EAT_DIMENSION2D);
    }
    if (s == "EAT_ENUM") {
        return static_cast<int>(EAT_ENUM);
    }
    if (s == "EAT_FLOATARRAY") {
        return static_cast<int>(EAT_FLOATARRAY);
    }
    if (s == "EAT_FLOAT") {
        return static_cast<int>(EAT_FLOAT);
    }
    if (s == "EAT_INTARRAY") {
        return static_cast<int>(EAT_INTARRAY);
    }
    if (s == "EAT_INT") {
        return static_cast<int>(EAT_INT);
    }
    if (s == "EAT_LINE2D") {
        return static_cast<int>(EAT_LINE2D);
    }
    if (s == "EAT_LINE3D") {
        return static_cast<int>(EAT_LINE3D);
    }
    if (s == "EAT_MATRIX") {
        return static_cast<int>(EAT_MATRIX);
    }
    if (s == "EAT_PLANE") {
        return static_cast<int>(EAT_PLANE);
    }
    if (s == "EAT_POSITION2D") {
        return static_cast<int>(EAT_POSITION2D);
    }
    if (s == "EAT_QUATERNION") {
        return static_cast<int>(EAT_QUATERNION);
    }
    if (s == "EAT_RECT") {
        return static_cast<int>(EAT_RECT);
    }
    if (s == "EAT_STRING") {
        return static_cast<int>(EAT_STRING);
    }
    if (s == "EAT_STRINGWARRAY") {
        return static_cast<int>(EAT_STRINGWARRAY);
    }
    if (s == "EAT_TEXTURE") {
        return static_cast<int>(EAT_TEXTURE);
    }
    if (s == "EAT_TRIANGLE3D") {
        return static_cast<int>(EAT_TRIANGLE3D);
    }
    if (s == "EAT_UNKNOWN") {
        return static_cast<int>(EAT_UNKNOWN);
    }
    if (s == "EAT_USER_POINTER") {
        return static_cast<int>(EAT_USER_POINTER);
    }
    if (s == "EAT_VECTOR2D") {
        return static_cast<int>(EAT_VECTOR2D);
    }
    if (s == "EAT_VECTOR3D") {
        return static_cast<int>(EAT_VECTOR3D);
    }
    if (s == "EBAM_ANIMATED") {
        return static_cast<int>(EBAM_ANIMATED);
    }
    if (s == "EBAM_AUTOMATIC") {
        return static_cast<int>(EBAM_AUTOMATIC);
    }
    if (s == "EBAM_COUNT") {
        return static_cast<int>(EBAM_COUNT);
    }
    if (s == "EBAM_UNANIMATED") {
        return static_cast<int>(EBAM_UNANIMATED);
    }
    if (s == "EBF_DST_ALPHA") {
        return static_cast<int>(EBF_DST_ALPHA);
    }
    if (s == "EBF_DST_COLOR") {
        return static_cast<int>(EBF_DST_COLOR);
    }
    if (s == "EBF_ONE_MINUS_DST_ALPHA") {
        return static_cast<int>(EBF_ONE_MINUS_DST_ALPHA);
    }
    if (s == "EBF_ONE_MINUS_DST_COLOR") {
        return static_cast<int>(EBF_ONE_MINUS_DST_COLOR);
    }
    if (s == "EBF_ONE_MINUS_SRC_ALPHA") {
        return static_cast<int>(EBF_ONE_MINUS_SRC_ALPHA);
    }
    if (s == "EBF_ONE_MINUS_SRC_COLOR") {
        return static_cast<int>(EBF_ONE_MINUS_SRC_COLOR);
    }
    if (s == "EBF_ONE") {
        return static_cast<int>(EBF_ONE);
    }
    if (s == "EBF_SRC_ALPHA") {
        return static_cast<int>(EBF_SRC_ALPHA);
    }
    if (s == "EBF_SRC_ALPHA_SATURATE") {
        return static_cast<int>(EBF_SRC_ALPHA_SATURATE);
    }
    if (s == "EBF_SRC_COLOR") {
        return static_cast<int>(EBF_SRC_COLOR);
    }
    if (s == "EBF_ZERO") {
        return static_cast<int>(EBF_ZERO);
    }
    if (s == "EBO_ADD") {
        return static_cast<int>(EBO_ADD);
    }
    if (s == "EBO_MAX_ALPHA") {
        return static_cast<int>(EBO_MAX_ALPHA);
    }
    if (s == "EBO_MAX_FACTOR") {
        return static_cast<int>(EBO_MAX_FACTOR);
    }
    if (s == "EBO_MAX") {
        return static_cast<int>(EBO_MAX);
    }
    if (s == "EBO_MIN_ALPHA") {
        return static_cast<int>(EBO_MIN_ALPHA);
    }
    if (s == "EBO_MIN_FACTOR") {
        return static_cast<int>(EBO_MIN_FACTOR);
    }
    if (s == "EBO_MIN") {
        return static_cast<int>(EBO_MIN);
    }
    if (s == "EBO_NONE") {
        return static_cast<int>(EBO_NONE);
    }
    if (s == "EBO_REVSUBTRACT") {
        return static_cast<int>(EBO_REVSUBTRACT);
    }
    if (s == "EBO_SUBTRACT") {
        return static_cast<int>(EBO_SUBTRACT);
    }
    if (s == "EBSS_COUNT") {
        return static_cast<int>(EBSS_COUNT);
    }
    if (s == "EBSS_GLOBAL") {
        return static_cast<int>(EBSS_GLOBAL);
    }
    if (s == "EBSS_LOCAL") {
        return static_cast<int>(EBSS_LOCAL);
    }
    if (s == "EBT_INDEX") {
        return static_cast<int>(EBT_INDEX);
    }
    if (s == "EBT_NONE") {
        return static_cast<int>(EBT_NONE);
    }
    if (s == "EBT_VERTEX_AND_INDEX") {
        return static_cast<int>(EBT_VERTEX_AND_INDEX);
    }
    if (s == "EBT_VERTEX") {
        return static_cast<int>(EBT_VERTEX);
    }
    if (s == "ECCS_AMBIENT") {
        return static_cast<int>(ECCS_AMBIENT);
    }
    if (s == "ECCS_DIFFUSE") {
        return static_cast<int>(ECCS_DIFFUSE);
    }
    if (s == "ECCS_EMISSIVE") {
        return static_cast<int>(ECCS_EMISSIVE);
    }
    if (s == "ECCS_REFLECTIVE") {
        return static_cast<int>(ECCS_REFLECTIVE);
    }
    if (s == "ECCS_SPECULAR") {
        return static_cast<int>(ECCS_SPECULAR);
    }
    if (s == "ECCS_TRANSPARENT") {
        return static_cast<int>(ECCS_TRANSPARENT);
    }
    if (s == "ECF_A16B16G16R16F") {
        return static_cast<int>(ECF_A16B16G16R16F);
    }
    if (s == "ECF_A1R5G5B5") {
        return static_cast<int>(ECF_A1R5G5B5);
    }
    if (s == "ECF_A32B32G32R32F") {
        return static_cast<int>(ECF_A32B32G32R32F);
    }
    if (s == "ECF_A8R8G8B8") {
        return static_cast<int>(ECF_A8R8G8B8);
    }
    if (s == "ECF_G16R16F") {
        return static_cast<int>(ECF_G16R16F);
    }
    if (s == "ECF_G32R32F") {
        return static_cast<int>(ECF_G32R32F);
    }
    if (s == "ECFN_ALWAYS") {
        return static_cast<int>(ECFN_ALWAYS);
    }
    if (s == "ECFN_EQUAL") {
        return static_cast<int>(ECFN_EQUAL);
    }
    if (s == "ECFN_GREATEREQUAL") {
        return static_cast<int>(ECFN_GREATEREQUAL);
    }
    if (s == "ECFN_GREATER") {
        return static_cast<int>(ECFN_GREATER);
    }
    if (s == "ECFN_LESSEQUAL") {
        return static_cast<int>(ECFN_LESSEQUAL);
    }
    if (s == "ECFN_LESS") {
        return static_cast<int>(ECFN_LESS);
    }
    if (s == "ECFN_NEVER") {
        return static_cast<int>(ECFN_NEVER);
    }
    if (s == "ECFN_NOTEQUAL") {
        return static_cast<int>(ECFN_NOTEQUAL);
    }
    if (s == "ECF_R16F") {
        return static_cast<int>(ECF_R16F);
    }
    if (s == "ECF_R32F") {
        return static_cast<int>(ECF_R32F);
    }
    if (s == "ECF_R5G6B5") {
        return static_cast<int>(ECF_R5G6B5);
    }
    if (s == "ECF_R8G8B8") {
        return static_cast<int>(ECF_R8G8B8);
    }
    if (s == "ECF_UNKNOWN") {
        return static_cast<int>(ECF_UNKNOWN);
    }
    if (s == "ECGI_PER_MESH_AND_MATERIAL") {
        return static_cast<int>(ECGI_PER_MESH_AND_MATERIAL);
    }
    if (s == "ECGI_PER_MESH") {
        return static_cast<int>(ECGI_PER_MESH);
    }
    if (s == "ECIC_AMBIENT") {
        return static_cast<int>(ECIC_AMBIENT);
    }
    if (s == "ECIC_CUSTOM") {
        return static_cast<int>(ECIC_CUSTOM);
    }
    if (s == "ECIC_DIFFUSE") {
        return static_cast<int>(ECIC_DIFFUSE);
    }
    if (s == "ECIC_EMISSIVE") {
        return static_cast<int>(ECIC_EMISSIVE);
    }
    if (s == "ECIC_NONE") {
        return static_cast<int>(ECIC_NONE);
    }
    if (s == "ECI_COUNT") {
        return static_cast<int>(ECI_COUNT);
    }
    if (s == "ECI_CROSS") {
        return static_cast<int>(ECI_CROSS);
    }
    if (s == "ECIC_SPECULAR") {
        return static_cast<int>(ECIC_SPECULAR);
    }
    if (s == "ECI_HAND") {
        return static_cast<int>(ECI_HAND);
    }
    if (s == "ECI_HELP") {
        return static_cast<int>(ECI_HELP);
    }
    if (s == "ECI_IBEAM") {
        return static_cast<int>(ECI_IBEAM);
    }
    if (s == "ECI_NO") {
        return static_cast<int>(ECI_NO);
    }
    if (s == "ECI_NORMAL") {
        return static_cast<int>(ECI_NORMAL);
    }
    if (s == "ECI_SIZEALL") {
        return static_cast<int>(ECI_SIZEALL);
    }
    if (s == "ECI_SIZENESW") {
        return static_cast<int>(ECI_SIZENESW);
    }
    if (s == "ECI_SIZENS") {
        return static_cast<int>(ECI_SIZENS);
    }
    if (s == "ECI_SIZENWSE") {
        return static_cast<int>(ECI_SIZENWSE);
    }
    if (s == "ECI_SIZEWE") {
        return static_cast<int>(ECI_SIZEWE);
    }
    if (s == "ECI_UP") {
        return static_cast<int>(ECI_UP);
    }
    if (s == "ECI_WAIT") {
        return static_cast<int>(ECI_WAIT);
    }
    if (s == "ECM_AMBIENT") {
        return static_cast<int>(ECM_AMBIENT);
    }
    if (s == "ECMC_HIDE") {
        return static_cast<int>(ECMC_HIDE);
    }
    if (s == "ECMC_IGNORE") {
        return static_cast<int>(ECMC_IGNORE);
    }
    if (s == "ECMC_REMOVE") {
        return static_cast<int>(ECMC_REMOVE);
    }
    if (s == "ECM_DIFFUSE_AND_AMBIENT") {
        return static_cast<int>(ECM_DIFFUSE_AND_AMBIENT);
    }
    if (s == "ECM_DIFFUSE") {
        return static_cast<int>(ECM_DIFFUSE);
    }
    if (s == "ECM_EMISSIVE") {
        return static_cast<int>(ECM_EMISSIVE);
    }
    if (s == "ECM_NONE") {
        return static_cast<int>(ECM_NONE);
    }
    if (s == "ECM_SPECULAR") {
        return static_cast<int>(ECM_SPECULAR);
    }
    if (s == "ECOF_A_ONE") {
        return static_cast<int>(ECOF_A_ONE);
    }
    if (s == "ECOF_RGB_ZERO") {
        return static_cast<int>(ECOF_RGB_ZERO);
    }
    if (s == "ECP_ALL") {
        return static_cast<int>(ECP_ALL);
    }
    if (s == "ECP_ALPHA") {
        return static_cast<int>(ECP_ALPHA);
    }
    if (s == "ECP_BLUE") {
        return static_cast<int>(ECP_BLUE);
    }
    if (s == "ECPB_NONE") {
        return static_cast<int>(ECPB_NONE);
    }
    if (s == "ECPB_X11_CACHE_UPDATES") {
        return static_cast<int>(ECPB_X11_CACHE_UPDATES);
    }
    if (s == "ECP_GREEN") {
        return static_cast<int>(ECP_GREEN);
    }
    if (s == "ECP_NONE") {
        return static_cast<int>(ECP_NONE);
    }
    if (s == "ECP_RED") {
        return static_cast<int>(ECP_RED);
    }
    if (s == "ECP_RGB") {
        return static_cast<int>(ECP_RGB);
    }
    if (s == "ECTF_BLINN") {
        return static_cast<int>(ECTF_BLINN);
    }
    if (s == "ECTF_CONSTANT") {
        return static_cast<int>(ECTF_CONSTANT);
    }
    if (s == "ECTF_LAMBERT") {
        return static_cast<int>(ECTF_LAMBERT);
    }
    if (s == "ECTF_PHONG") {
        return static_cast<int>(ECTF_PHONG);
    }
    if (s == "EDS_BBOX_ALL") {
        return static_cast<int>(EDS_BBOX_ALL);
    }
    if (s == "EDS_BBOX_BUFFERS") {
        return static_cast<int>(EDS_BBOX_BUFFERS);
    }
    if (s == "EDS_BBOX") {
        return static_cast<int>(EDS_BBOX);
    }
    if (s == "EDS_FULL") {
        return static_cast<int>(EDS_FULL);
    }
    if (s == "EDS_HALF_TRANSPARENCY") {
        return static_cast<int>(EDS_HALF_TRANSPARENCY);
    }
    if (s == "EDS_MESH_WIRE_OVERLAY") {
        return static_cast<int>(EDS_MESH_WIRE_OVERLAY);
    }
    if (s == "EDS_NORMALS") {
        return static_cast<int>(EDS_NORMALS);
    }
    if (s == "EDS_OFF") {
        return static_cast<int>(EDS_OFF);
    }
    if (s == "EDS_SKELETON") {
        return static_cast<int>(EDS_SKELETON);
    }
    if (s == "EDT_BURNINGSVIDEO") {
        return static_cast<int>(EDT_BURNINGSVIDEO);
    }
    if (s == "EDT_COUNT") {
        return static_cast<int>(EDT_COUNT);
    }
    if (s == "EDT_DIRECT3D9") {
        return static_cast<int>(EDT_DIRECT3D9);
    }
    if (s == "EDT_NULL") {
        return static_cast<int>(EDT_NULL);
    }
    if (s == "EDT_OPENGL") {
        return static_cast<int>(EDT_OPENGL);
    }
    if (s == "EDT_SOFTWARE") {
        return static_cast<int>(EDT_SOFTWARE);
    }
    if (s == "EET_GUI_EVENT") {
        return static_cast<int>(EET_GUI_EVENT);
    }
    if (s == "EET_JOYSTICK_INPUT_EVENT") {
        return static_cast<int>(EET_JOYSTICK_INPUT_EVENT);
    }
    if (s == "EET_KEY_INPUT_EVENT") {
        return static_cast<int>(EET_KEY_INPUT_EVENT);
    }
    if (s == "EET_LOG_TEXT_EVENT") {
        return static_cast<int>(EET_LOG_TEXT_EVENT);
    }
    if (s == "EET_MOUSE_INPUT_EVENT") {
        return static_cast<int>(EET_MOUSE_INPUT_EVENT);
    }
    if (s == "EET_USER_EVENT") {
        return static_cast<int>(EET_USER_EVENT);
    }
    if (s == "EFAT_FOLDER") {
        return static_cast<int>(EFAT_FOLDER);
    }
    if (s == "EFAT_GZIP") {
        return static_cast<int>(EFAT_GZIP);
    }
    if (s == "EFAT_NPK") {
        return static_cast<int>(EFAT_NPK);
    }
    if (s == "EFAT_PAK") {
        return static_cast<int>(EFAT_PAK);
    }
    if (s == "EFAT_TAR") {
        return static_cast<int>(EFAT_TAR);
    }
    if (s == "EFAT_UNKNOWN") {
        return static_cast<int>(EFAT_UNKNOWN);
    }
    if (s == "EFAT_WAD") {
        return static_cast<int>(EFAT_WAD);
    }
    if (s == "EFAT_ZIP") {
        return static_cast<int>(EFAT_ZIP);
    }
    if (s == "EFT_FOG_EXP2") {
        return static_cast<int>(EFT_FOG_EXP2);
    }
    if (s == "EFT_FOG_EXP") {
        return static_cast<int>(EFT_FOG_EXP);
    }
    if (s == "EFT_FOG_LINEAR") {
        return static_cast<int>(EFT_FOG_LINEAR);
    }
    if (s == "EGBS_BUTTON_DOWN") {
        return static_cast<int>(EGBS_BUTTON_DOWN);
    }
    if (s == "EGBS_BUTTON_FOCUSED") {
        return static_cast<int>(EGBS_BUTTON_FOCUSED);
    }
    if (s == "EGBS_BUTTON_MOUSE_OFF") {
        return static_cast<int>(EGBS_BUTTON_MOUSE_OFF);
    }
    if (s == "EGBS_BUTTON_MOUSE_OVER") {
        return static_cast<int>(EGBS_BUTTON_MOUSE_OVER);
    }
    if (s == "EGBS_BUTTON_NOT_FOCUSED") {
        return static_cast<int>(EGBS_BUTTON_NOT_FOCUSED);
    }
    if (s == "EGBS_BUTTON_UP") {
        return static_cast<int>(EGBS_BUTTON_UP);
    }
    if (s == "EGBS_COUNT") {
        return static_cast<int>(EGBS_COUNT);
    }
    if (s == "EGCO_ASCENDING") {
        return static_cast<int>(EGCO_ASCENDING);
    }
    if (s == "EGCO_COUNT") {
        return static_cast<int>(EGCO_COUNT);
    }
    if (s == "EGCO_CUSTOM") {
        return static_cast<int>(EGCO_CUSTOM);
    }
    if (s == "EGCO_DESCENDING") {
        return static_cast<int>(EGCO_DESCENDING);
    }
    if (s == "EGCO_FLIP_ASCENDING_DESCENDING") {
        return static_cast<int>(EGCO_FLIP_ASCENDING_DESCENDING);
    }
    if (s == "EGCO_NONE") {
        return static_cast<int>(EGCO_NONE);
    }
    if (s == "EGDC_3D_DARK_SHADOW") {
        return static_cast<int>(EGDC_3D_DARK_SHADOW);
    }
    if (s == "EGDC_3D_FACE") {
        return static_cast<int>(EGDC_3D_FACE);
    }
    if (s == "EGDC_3D_HIGH_LIGHT") {
        return static_cast<int>(EGDC_3D_HIGH_LIGHT);
    }
    if (s == "EGDC_3D_LIGHT") {
        return static_cast<int>(EGDC_3D_LIGHT);
    }
    if (s == "EGDC_3D_SHADOW") {
        return static_cast<int>(EGDC_3D_SHADOW);
    }
    if (s == "EGDC_ACTIVE_BORDER") {
        return static_cast<int>(EGDC_ACTIVE_BORDER);
    }
    if (s == "EGDC_ACTIVE_CAPTION") {
        return static_cast<int>(EGDC_ACTIVE_CAPTION);
    }
    if (s == "EGDC_APP_WORKSPACE") {
        return static_cast<int>(EGDC_APP_WORKSPACE);
    }
    if (s == "EGDC_BUTTON_TEXT") {
        return static_cast<int>(EGDC_BUTTON_TEXT);
    }
    if (s == "EGDC_COUNT") {
        return static_cast<int>(EGDC_COUNT);
    }
    if (s == "EGDC_EDITABLE") {
        return static_cast<int>(EGDC_EDITABLE);
    }
    if (s == "EGDC_FOCUSED_EDITABLE") {
        return static_cast<int>(EGDC_FOCUSED_EDITABLE);
    }
    if (s == "EGDC_GRAY_EDITABLE") {
        return static_cast<int>(EGDC_GRAY_EDITABLE);
    }
    if (s == "EGDC_GRAY_TEXT") {
        return static_cast<int>(EGDC_GRAY_TEXT);
    }
    if (s == "EGDC_GRAY_WINDOW_SYMBOL") {
        return static_cast<int>(EGDC_GRAY_WINDOW_SYMBOL);
    }
    if (s == "EGDC_HIGH_LIGHT") {
        return static_cast<int>(EGDC_HIGH_LIGHT);
    }
    if (s == "EGDC_HIGH_LIGHT_TEXT") {
        return static_cast<int>(EGDC_HIGH_LIGHT_TEXT);
    }
    if (s == "EGDC_ICON_HIGH_LIGHT") {
        return static_cast<int>(EGDC_ICON_HIGH_LIGHT);
    }
    if (s == "EGDC_ICON") {
        return static_cast<int>(EGDC_ICON);
    }
    if (s == "EGDC_INACTIVE_BORDER") {
        return static_cast<int>(EGDC_INACTIVE_BORDER);
    }
    if (s == "EGDC_INACTIVE_CAPTION") {
        return static_cast<int>(EGDC_INACTIVE_CAPTION);
    }
    if (s == "EGDC_SCROLLBAR") {
        return static_cast<int>(EGDC_SCROLLBAR);
    }
    if (s == "EGDC_TOOLTIP_BACKGROUND") {
        return static_cast<int>(EGDC_TOOLTIP_BACKGROUND);
    }
    if (s == "EGDC_TOOLTIP") {
        return static_cast<int>(EGDC_TOOLTIP);
    }
    if (s == "EGDC_WINDOW") {
        return static_cast<int>(EGDC_WINDOW);
    }
    if (s == "EGDC_WINDOW_SYMBOL") {
        return static_cast<int>(EGDC_WINDOW_SYMBOL);
    }
    if (s == "EGDF_BUTTON") {
        return static_cast<int>(EGDF_BUTTON);
    }
    if (s == "EGDF_COUNT") {
        return static_cast<int>(EGDF_COUNT);
    }
    if (s == "EGDF_DEFAULT") {
        return static_cast<int>(EGDF_DEFAULT);
    }
    if (s == "EGDF_MENU") {
        return static_cast<int>(EGDF_MENU);
    }
    if (s == "EGDF_TOOLTIP") {
        return static_cast<int>(EGDF_TOOLTIP);
    }
    if (s == "EGDF_WINDOW") {
        return static_cast<int>(EGDF_WINDOW);
    }
    if (s == "EGDI_CHECK_BOX_CHECKED") {
        return static_cast<int>(EGDI_CHECK_BOX_CHECKED);
    }
    if (s == "EGDI_COLLAPSE") {
        return static_cast<int>(EGDI_COLLAPSE);
    }
    if (s == "EGDI_COUNT") {
        return static_cast<int>(EGDI_COUNT);
    }
    if (s == "EGDI_CURSOR_DOWN") {
        return static_cast<int>(EGDI_CURSOR_DOWN);
    }
    if (s == "EGDI_CURSOR_LEFT") {
        return static_cast<int>(EGDI_CURSOR_LEFT);
    }
    if (s == "EGDI_CURSOR_RIGHT") {
        return static_cast<int>(EGDI_CURSOR_RIGHT);
    }
    if (s == "EGDI_CURSOR_UP") {
        return static_cast<int>(EGDI_CURSOR_UP);
    }
    if (s == "EGDI_DIRECTORY") {
        return static_cast<int>(EGDI_DIRECTORY);
    }
    if (s == "EGDI_DROP_DOWN") {
        return static_cast<int>(EGDI_DROP_DOWN);
    }
    if (s == "EGDI_EXPAND") {
        return static_cast<int>(EGDI_EXPAND);
    }
    if (s == "EGDI_FILE") {
        return static_cast<int>(EGDI_FILE);
    }
    if (s == "EGDI_MENU_MORE") {
        return static_cast<int>(EGDI_MENU_MORE);
    }
    if (s == "EGDI_MORE_DOWN") {
        return static_cast<int>(EGDI_MORE_DOWN);
    }
    if (s == "EGDI_MORE_LEFT") {
        return static_cast<int>(EGDI_MORE_LEFT);
    }
    if (s == "EGDI_MORE_RIGHT") {
        return static_cast<int>(EGDI_MORE_RIGHT);
    }
    if (s == "EGDI_MORE_UP") {
        return static_cast<int>(EGDI_MORE_UP);
    }
    if (s == "EGDI_RADIO_BUTTON_CHECKED") {
        return static_cast<int>(EGDI_RADIO_BUTTON_CHECKED);
    }
    if (s == "EGDI_SMALL_CURSOR_DOWN") {
        return static_cast<int>(EGDI_SMALL_CURSOR_DOWN);
    }
    if (s == "EGDI_SMALL_CURSOR_UP") {
        return static_cast<int>(EGDI_SMALL_CURSOR_UP);
    }
    if (s == "EGDI_WINDOW_CLOSE") {
        return static_cast<int>(EGDI_WINDOW_CLOSE);
    }
    if (s == "EGDI_WINDOW_MAXIMIZE") {
        return static_cast<int>(EGDI_WINDOW_MAXIMIZE);
    }
    if (s == "EGDI_WINDOW_MINIMIZE") {
        return static_cast<int>(EGDI_WINDOW_MINIMIZE);
    }
    if (s == "EGDI_WINDOW_RESIZE") {
        return static_cast<int>(EGDI_WINDOW_RESIZE);
    }
    if (s == "EGDI_WINDOW_RESTORE") {
        return static_cast<int>(EGDI_WINDOW_RESTORE);
    }
    if (s == "EGDS_BUTTON_HEIGHT") {
        return static_cast<int>(EGDS_BUTTON_HEIGHT);
    }
    if (s == "EGDS_BUTTON_PRESSED_IMAGE_OFFSET_X") {
        return static_cast<int>(EGDS_BUTTON_PRESSED_IMAGE_OFFSET_X);
    }
    if (s == "EGDS_BUTTON_PRESSED_IMAGE_OFFSET_Y") {
        return static_cast<int>(EGDS_BUTTON_PRESSED_IMAGE_OFFSET_Y);
    }
    if (s == "EGDS_BUTTON_PRESSED_TEXT_OFFSET_X") {
        return static_cast<int>(EGDS_BUTTON_PRESSED_TEXT_OFFSET_X);
    }
    if (s == "EGDS_BUTTON_PRESSED_TEXT_OFFSET_Y") {
        return static_cast<int>(EGDS_BUTTON_PRESSED_TEXT_OFFSET_Y);
    }
    if (s == "EGDS_BUTTON_WIDTH") {
        return static_cast<int>(EGDS_BUTTON_WIDTH);
    }
    if (s == "EGDS_CHECK_BOX_WIDTH") {
        return static_cast<int>(EGDS_CHECK_BOX_WIDTH);
    }
    if (s == "EGDS_COUNT") {
        return static_cast<int>(EGDS_COUNT);
    }
    if (s == "EGDS_MENU_HEIGHT") {
        return static_cast<int>(EGDS_MENU_HEIGHT);
    }
    if (s == "EGDS_MESSAGE_BOX_GAP_SPACE") {
        return static_cast<int>(EGDS_MESSAGE_BOX_GAP_SPACE);
    }
    if (s == "EGDS_MESSAGE_BOX_HEIGHT") {
        return static_cast<int>(EGDS_MESSAGE_BOX_HEIGHT);
    }
    if (s == "EGDS_MESSAGE_BOX_MAX_TEXT_HEIGHT") {
        return static_cast<int>(EGDS_MESSAGE_BOX_MAX_TEXT_HEIGHT);
    }
    if (s == "EGDS_MESSAGE_BOX_MAX_TEXT_WIDTH") {
        return static_cast<int>(EGDS_MESSAGE_BOX_MAX_TEXT_WIDTH);
    }
    if (s == "EGDS_MESSAGE_BOX_MIN_TEXT_HEIGHT") {
        return static_cast<int>(EGDS_MESSAGE_BOX_MIN_TEXT_HEIGHT);
    }
    if (s == "EGDS_MESSAGE_BOX_MIN_TEXT_WIDTH") {
        return static_cast<int>(EGDS_MESSAGE_BOX_MIN_TEXT_WIDTH);
    }
    if (s == "EGDS_MESSAGE_BOX_WIDTH") {
        return static_cast<int>(EGDS_MESSAGE_BOX_WIDTH);
    }
    if (s == "EGDS_SCROLLBAR_SIZE") {
        return static_cast<int>(EGDS_SCROLLBAR_SIZE);
    }
    if (s == "EGDS_TEXT_DISTANCE_X") {
        return static_cast<int>(EGDS_TEXT_DISTANCE_X);
    }
    if (s == "EGDS_TEXT_DISTANCE_Y") {
        return static_cast<int>(EGDS_TEXT_DISTANCE_Y);
    }
    if (s == "EGDS_TITLEBARTEXT_DISTANCE_X") {
        return static_cast<int>(EGDS_TITLEBARTEXT_DISTANCE_X);
    }
    if (s == "EGDS_TITLEBARTEXT_DISTANCE_Y") {
        return static_cast<int>(EGDS_TITLEBARTEXT_DISTANCE_Y);
    }
    if (s == "EGDS_WINDOW_BUTTON_WIDTH") {
        return static_cast<int>(EGDS_WINDOW_BUTTON_WIDTH);
    }
    if (s == "EGDT_COUNT") {
        return static_cast<int>(EGDT_COUNT);
    }
    if (s == "EGDT_MSG_BOX_CANCEL") {
        return static_cast<int>(EGDT_MSG_BOX_CANCEL);
    }
    if (s == "EGDT_MSG_BOX_NO") {
        return static_cast<int>(EGDT_MSG_BOX_NO);
    }
    if (s == "EGDT_MSG_BOX_OK") {
        return static_cast<int>(EGDT_MSG_BOX_OK);
    }
    if (s == "EGDT_MSG_BOX_YES") {
        return static_cast<int>(EGDT_MSG_BOX_YES);
    }
    if (s == "EGDT_WINDOW_CLOSE") {
        return static_cast<int>(EGDT_WINDOW_CLOSE);
    }
    if (s == "EGDT_WINDOW_MAXIMIZE") {
        return static_cast<int>(EGDT_WINDOW_MAXIMIZE);
    }
    if (s == "EGDT_WINDOW_MINIMIZE") {
        return static_cast<int>(EGDT_WINDOW_MINIMIZE);
    }
    if (s == "EGDT_WINDOW_RESTORE") {
        return static_cast<int>(EGDT_WINDOW_RESTORE);
    }
    if (s == "EGET_BUTTON_CLICKED") {
        return static_cast<int>(EGET_BUTTON_CLICKED);
    }
    if (s == "EGET_CHECKBOX_CHANGED") {
        return static_cast<int>(EGET_CHECKBOX_CHANGED);
    }
    if (s == "EGET_COMBO_BOX_CHANGED") {
        return static_cast<int>(EGET_COMBO_BOX_CHANGED);
    }
    if (s == "EGET_COUNT") {
        return static_cast<int>(EGET_COUNT);
    }
    if (s == "EGET_DIRECTORY_SELECTED") {
        return static_cast<int>(EGET_DIRECTORY_SELECTED);
    }
    if (s == "EGET_EDITBOX_CHANGED") {
        return static_cast<int>(EGET_EDITBOX_CHANGED);
    }
    if (s == "EGET_EDITBOX_ENTER") {
        return static_cast<int>(EGET_EDITBOX_ENTER);
    }
    if (s == "EGET_EDITBOX_MARKING_CHANGED") {
        return static_cast<int>(EGET_EDITBOX_MARKING_CHANGED);
    }
    if (s == "EGET_ELEMENT_CLOSED") {
        return static_cast<int>(EGET_ELEMENT_CLOSED);
    }
    if (s == "EGET_ELEMENT_FOCUSED") {
        return static_cast<int>(EGET_ELEMENT_FOCUSED);
    }
    if (s == "EGET_ELEMENT_FOCUS_LOST") {
        return static_cast<int>(EGET_ELEMENT_FOCUS_LOST);
    }
    if (s == "EGET_ELEMENT_HOVERED") {
        return static_cast<int>(EGET_ELEMENT_HOVERED);
    }
    if (s == "EGET_ELEMENT_LEFT") {
        return static_cast<int>(EGET_ELEMENT_LEFT);
    }
    if (s == "EGET_FILE_CHOOSE_DIALOG_CANCELLED") {
        return static_cast<int>(EGET_FILE_CHOOSE_DIALOG_CANCELLED);
    }
    if (s == "EGET_FILE_SELECTED") {
        return static_cast<int>(EGET_FILE_SELECTED);
    }
    if (s == "EGET_LISTBOX_CHANGED") {
        return static_cast<int>(EGET_LISTBOX_CHANGED);
    }
    if (s == "EGET_LISTBOX_SELECTED_AGAIN") {
        return static_cast<int>(EGET_LISTBOX_SELECTED_AGAIN);
    }
    if (s == "EGET_MENU_ITEM_SELECTED") {
        return static_cast<int>(EGET_MENU_ITEM_SELECTED);
    }
    if (s == "EGET_MESSAGEBOX_CANCEL") {
        return static_cast<int>(EGET_MESSAGEBOX_CANCEL);
    }
    if (s == "EGET_MESSAGEBOX_NO") {
        return static_cast<int>(EGET_MESSAGEBOX_NO);
    }
    if (s == "EGET_MESSAGEBOX_OK") {
        return static_cast<int>(EGET_MESSAGEBOX_OK);
    }
    if (s == "EGET_MESSAGEBOX_YES") {
        return static_cast<int>(EGET_MESSAGEBOX_YES);
    }
    if (s == "EGET_SCROLL_BAR_CHANGED") {
        return static_cast<int>(EGET_SCROLL_BAR_CHANGED);
    }
    if (s == "EGET_SPINBOX_CHANGED") {
        return static_cast<int>(EGET_SPINBOX_CHANGED);
    }
    if (s == "EGET_TAB_CHANGED") {
        return static_cast<int>(EGET_TAB_CHANGED);
    }
    if (s == "EGET_TABLE_CHANGED") {
        return static_cast<int>(EGET_TABLE_CHANGED);
    }
    if (s == "EGET_TABLE_HEADER_CHANGED") {
        return static_cast<int>(EGET_TABLE_HEADER_CHANGED);
    }
    if (s == "EGET_TABLE_SELECTED_AGAIN") {
        return static_cast<int>(EGET_TABLE_SELECTED_AGAIN);
    }
    if (s == "EGET_TREEVIEW_NODE_COLLAPSE") {
        return static_cast<int>(EGET_TREEVIEW_NODE_COLLAPSE);
    }
    if (s == "EGET_TREEVIEW_NODE_COLLAPS") {
        return static_cast<int>(EGET_TREEVIEW_NODE_COLLAPS);
    }
    if (s == "EGET_TREEVIEW_NODE_DESELECT") {
        return static_cast<int>(EGET_TREEVIEW_NODE_DESELECT);
    }
    if (s == "EGET_TREEVIEW_NODE_EXPAND") {
        return static_cast<int>(EGET_TREEVIEW_NODE_EXPAND);
    }
    if (s == "EGET_TREEVIEW_NODE_SELECT") {
        return static_cast<int>(EGET_TREEVIEW_NODE_SELECT);
    }
    if (s == "EGFT_BITMAP") {
        return static_cast<int>(EGFT_BITMAP);
    }
    if (s == "EGFT_CUSTOM") {
        return static_cast<int>(EGFT_CUSTOM);
    }
    if (s == "EGFT_OS") {
        return static_cast<int>(EGFT_OS);
    }
    if (s == "EGFT_VECTOR") {
        return static_cast<int>(EGFT_VECTOR);
    }
    if (s == "EGOM_ASCENDING") {
        return static_cast<int>(EGOM_ASCENDING);
    }
    if (s == "EGOM_COUNT") {
        return static_cast<int>(EGOM_COUNT);
    }
    if (s == "EGOM_DESCENDING") {
        return static_cast<int>(EGOM_DESCENDING);
    }
    if (s == "EGOM_NONE") {
        return static_cast<int>(EGOM_NONE);
    }
    if (s == "EGSL_CG") {
        return static_cast<int>(EGSL_CG);
    }
    if (s == "EGSL_DEFAULT") {
        return static_cast<int>(EGSL_DEFAULT);
    }
    if (s == "EGST_BURNING_SKIN") {
        return static_cast<int>(EGST_BURNING_SKIN);
    }
    if (s == "EGST_GS_4_0") {
        return static_cast<int>(EGST_GS_4_0);
    }
    if (s == "EGST_UNKNOWN") {
        return static_cast<int>(EGST_UNKNOWN);
    }
    if (s == "EGST_WINDOWS_CLASSIC") {
        return static_cast<int>(EGST_WINDOWS_CLASSIC);
    }
    if (s == "EGST_WINDOWS_METALLIC") {
        return static_cast<int>(EGST_WINDOWS_METALLIC);
    }
    if (s == "EGTDF_ACTIVE_ROW") {
        return static_cast<int>(EGTDF_ACTIVE_ROW);
    }
    if (s == "EGTDF_COLUMNS") {
        return static_cast<int>(EGTDF_COLUMNS);
    }
    if (s == "EGTDF_COUNT") {
        return static_cast<int>(EGTDF_COUNT);
    }
    if (s == "EGTDF_ROWS") {
        return static_cast<int>(EGTDF_ROWS);
    }
    if (s == "EGUIA_CENTER") {
        return static_cast<int>(EGUIA_CENTER);
    }
    if (s == "EGUIA_LOWERRIGHT") {
        return static_cast<int>(EGUIA_LOWERRIGHT);
    }
    if (s == "EGUIA_SCALE") {
        return static_cast<int>(EGUIA_SCALE);
    }
    if (s == "EGUIA_UPPERLEFT") {
        return static_cast<int>(EGUIA_UPPERLEFT);
    }
    if (s == "EGUIET_BUTTON") {
        return static_cast<int>(EGUIET_BUTTON);
    }
    if (s == "EGUIET_CHECK_BOX") {
        return static_cast<int>(EGUIET_CHECK_BOX);
    }
    if (s == "EGUIET_COLOR_SELECT_DIALOG") {
        return static_cast<int>(EGUIET_COLOR_SELECT_DIALOG);
    }
    if (s == "EGUIET_COMBO_BOX") {
        return static_cast<int>(EGUIET_COMBO_BOX);
    }
    if (s == "EGUIET_CONTEXT_MENU") {
        return static_cast<int>(EGUIET_CONTEXT_MENU);
    }
    if (s == "EGUIET_COUNT") {
        return static_cast<int>(EGUIET_COUNT);
    }
    if (s == "EGUIET_EDIT_BOX") {
        return static_cast<int>(EGUIET_EDIT_BOX);
    }
    if (s == "EGUIET_ELEMENT") {
        return static_cast<int>(EGUIET_ELEMENT);
    }
    if (s == "EGUIET_FILE_OPEN_DIALOG") {
        return static_cast<int>(EGUIET_FILE_OPEN_DIALOG);
    }
    if (s == "EGUIET_IMAGE") {
        return static_cast<int>(EGUIET_IMAGE);
    }
    if (s == "EGUIET_IN_OUT_FADER") {
        return static_cast<int>(EGUIET_IN_OUT_FADER);
    }
    if (s == "EGUIET_LIST_BOX") {
        return static_cast<int>(EGUIET_LIST_BOX);
    }
    if (s == "EGUIET_MENU") {
        return static_cast<int>(EGUIET_MENU);
    }
    if (s == "EGUIET_MESH_VIEWER") {
        return static_cast<int>(EGUIET_MESH_VIEWER);
    }
    if (s == "EGUIET_MESSAGE_BOX") {
        return static_cast<int>(EGUIET_MESSAGE_BOX);
    }
    if (s == "EGUIET_MODAL_SCREEN") {
        return static_cast<int>(EGUIET_MODAL_SCREEN);
    }
    if (s == "EGUIET_ROOT") {
        return static_cast<int>(EGUIET_ROOT);
    }
    if (s == "EGUIET_SCROLL_BAR") {
        return static_cast<int>(EGUIET_SCROLL_BAR);
    }
    if (s == "EGUIET_SPIN_BOX") {
        return static_cast<int>(EGUIET_SPIN_BOX);
    }
    if (s == "EGUIET_STATIC_TEXT") {
        return static_cast<int>(EGUIET_STATIC_TEXT);
    }
    if (s == "EGUIET_TAB_CONTROL") {
        return static_cast<int>(EGUIET_TAB_CONTROL);
    }
    if (s == "EGUIET_TABLE") {
        return static_cast<int>(EGUIET_TABLE);
    }
    if (s == "EGUIET_TAB") {
        return static_cast<int>(EGUIET_TAB);
    }
    if (s == "EGUIET_TOOL_BAR") {
        return static_cast<int>(EGUIET_TOOL_BAR);
    }
    if (s == "EGUIET_TREE_VIEW") {
        return static_cast<int>(EGUIET_TREE_VIEW);
    }
    if (s == "EGUIET_WINDOW") {
        return static_cast<int>(EGUIET_WINDOW);
    }
    if (s == "EGUI_LBC_COUNT") {
        return static_cast<int>(EGUI_LBC_COUNT);
    }
    if (s == "EGUI_LBC_ICON_HIGHLIGHT") {
        return static_cast<int>(EGUI_LBC_ICON_HIGHLIGHT);
    }
    if (s == "EGUI_LBC_ICON") {
        return static_cast<int>(EGUI_LBC_ICON);
    }
    if (s == "EGUI_LBC_TEXT_HIGHLIGHT") {
        return static_cast<int>(EGUI_LBC_TEXT_HIGHLIGHT);
    }
    if (s == "EGUI_LBC_TEXT") {
        return static_cast<int>(EGUI_LBC_TEXT);
    }
    if (s == "EHM_DYNAMIC") {
        return static_cast<int>(EHM_DYNAMIC);
    }
    if (s == "EHM_NEVER") {
        return static_cast<int>(EHM_NEVER);
    }
    if (s == "EHM_STATIC") {
        return static_cast<int>(EHM_STATIC);
    }
    if (s == "EHM_STREAM") {
        return static_cast<int>(EHM_STREAM);
    }
    if (s == "EIDT_BEST") {
        return static_cast<int>(EIDT_BEST);
    }
    if (s == "EIDT_CONSOLE") {
        return static_cast<int>(EIDT_CONSOLE);
    }
    if (s == "EIDT_FRAMEBUFFER") {
        return static_cast<int>(EIDT_FRAMEBUFFER);
    }
    if (s == "EIDT_OSX") {
        return static_cast<int>(EIDT_OSX);
    }
    if (s == "EIDT_SDL") {
        return static_cast<int>(EIDT_SDL);
    }
    if (s == "EIDT_WIN32") {
        return static_cast<int>(EIDT_WIN32);
    }
    if (s == "EIDT_WINCE") {
        return static_cast<int>(EIDT_WINCE);
    }
    if (s == "EIDT_X11") {
        return static_cast<int>(EIDT_X11);
    }
    if (s == "EIM_CONSTANT") {
        return static_cast<int>(EIM_CONSTANT);
    }
    if (s == "EIM_COUNT") {
        return static_cast<int>(EIM_COUNT);
    }
    if (s == "EIM_LINEAR") {
        return static_cast<int>(EIM_LINEAR);
    }
    if (s == "EIT_16BIT") {
        return static_cast<int>(EIT_16BIT);
    }
    if (s == "EIT_32BIT") {
        return static_cast<int>(EIT_32BIT);
    }
    if (s == "EJUOR_CONTROL") {
        return static_cast<int>(EJUOR_CONTROL);
    }
    if (s == "EJUOR_NONE") {
        return static_cast<int>(EJUOR_NONE);
    }
    if (s == "EJUOR_READ") {
        return static_cast<int>(EJUOR_READ);
    }
    if (s == "EKA_COUNT") {
        return static_cast<int>(EKA_COUNT);
    }
    if (s == "EKA_CROUCH") {
        return static_cast<int>(EKA_CROUCH);
    }
    if (s == "EKA_FORCE_32BIT") {
        return static_cast<int>(EKA_FORCE_32BIT);
    }
    if (s == "EKA_JUMP_UP") {
        return static_cast<int>(EKA_JUMP_UP);
    }
    if (s == "EKA_MOVE_BACKWARD") {
        return static_cast<int>(EKA_MOVE_BACKWARD);
    }
    if (s == "EKA_MOVE_FORWARD") {
        return static_cast<int>(EKA_MOVE_FORWARD);
    }
    if (s == "EKA_STRAFE_LEFT") {
        return static_cast<int>(EKA_STRAFE_LEFT);
    }
    if (s == "EKA_STRAFE_RIGHT") {
        return static_cast<int>(EKA_STRAFE_RIGHT);
    }
    if (s == "ELL_DEBUG") {
        return static_cast<int>(ELL_DEBUG);
    }
    if (s == "ELL_ERROR") {
        return static_cast<int>(ELL_ERROR);
    }
    if (s == "ELL_INFORMATION") {
        return static_cast<int>(ELL_INFORMATION);
    }
    if (s == "ELL_NONE") {
        return static_cast<int>(ELL_NONE);
    }
    if (s == "ELL_WARNING") {
        return static_cast<int>(ELL_WARNING);
    }
    if (s == "ELR_DEVICE") {
        return static_cast<int>(ELR_DEVICE);
    }
    if (s == "ELR_HW_BUFFERS") {
        return static_cast<int>(ELR_HW_BUFFERS);
    }
    if (s == "ELR_RTTS") {
        return static_cast<int>(ELR_RTTS);
    }
    if (s == "ELR_TEXTURES") {
        return static_cast<int>(ELR_TEXTURES);
    }
    if (s == "ELT_COUNT") {
        return static_cast<int>(ELT_COUNT);
    }
    if (s == "ELT_DIRECTIONAL") {
        return static_cast<int>(ELT_DIRECTIONAL);
    }
    if (s == "ELT_POINT") {
        return static_cast<int>(ELT_POINT);
    }
    if (s == "ELT_SPOT") {
        return static_cast<int>(ELT_SPOT);
    }
    if (s == "EMAT_ATTACK") {
        return static_cast<int>(EMAT_ATTACK);
    }
    if (s == "EMAT_BOOM") {
        return static_cast<int>(EMAT_BOOM);
    }
    if (s == "EMAT_COUNT") {
        return static_cast<int>(EMAT_COUNT);
    }
    if (s == "EMAT_CROUCH_ATTACK") {
        return static_cast<int>(EMAT_CROUCH_ATTACK);
    }
    if (s == "EMAT_CROUCH_DEATH") {
        return static_cast<int>(EMAT_CROUCH_DEATH);
    }
    if (s == "EMAT_CROUCH_PAIN") {
        return static_cast<int>(EMAT_CROUCH_PAIN);
    }
    if (s == "EMAT_CROUCH_STAND") {
        return static_cast<int>(EMAT_CROUCH_STAND);
    }
    if (s == "EMAT_CROUCH_WALK") {
        return static_cast<int>(EMAT_CROUCH_WALK);
    }
    if (s == "EMAT_DEATH_FALLBACK") {
        return static_cast<int>(EMAT_DEATH_FALLBACK);
    }
    if (s == "EMAT_DEATH_FALLBACKSLOW") {
        return static_cast<int>(EMAT_DEATH_FALLBACKSLOW);
    }
    if (s == "EMAT_DEATH_FALLFORWARD") {
        return static_cast<int>(EMAT_DEATH_FALLFORWARD);
    }
    if (s == "EMAT_FALLBACK") {
        return static_cast<int>(EMAT_FALLBACK);
    }
    if (s == "EMAT_FLIP") {
        return static_cast<int>(EMAT_FLIP);
    }
    if (s == "EMAT_JUMP") {
        return static_cast<int>(EMAT_JUMP);
    }
    if (s == "EMAT_PAIN_A") {
        return static_cast<int>(EMAT_PAIN_A);
    }
    if (s == "EMAT_PAIN_B") {
        return static_cast<int>(EMAT_PAIN_B);
    }
    if (s == "EMAT_PAIN_C") {
        return static_cast<int>(EMAT_PAIN_C);
    }
    if (s == "EMAT_POINT") {
        return static_cast<int>(EMAT_POINT);
    }
    if (s == "EMAT_RUN") {
        return static_cast<int>(EMAT_RUN);
    }
    if (s == "EMAT_SALUTE") {
        return static_cast<int>(EMAT_SALUTE);
    }
    if (s == "EMAT_STAND") {
        return static_cast<int>(EMAT_STAND);
    }
    if (s == "EMAT_WAVE") {
        return static_cast<int>(EMAT_WAVE);
    }
    if (s == "EMBF_CANCEL") {
        return static_cast<int>(EMBF_CANCEL);
    }
    if (s == "EMBF_FORCE_32BIT") {
        return static_cast<int>(EMBF_FORCE_32BIT);
    }
    if (s == "EMBF_NO") {
        return static_cast<int>(EMBF_NO);
    }
    if (s == "EMBF_OK") {
        return static_cast<int>(EMBF_OK);
    }
    if (s == "EMBF_YES") {
        return static_cast<int>(EMBF_YES);
    }
    if (s == "EMBSM_EXTRA1") {
        return static_cast<int>(EMBSM_EXTRA1);
    }
    if (s == "EMBSM_EXTRA2") {
        return static_cast<int>(EMBSM_EXTRA2);
    }
    if (s == "EMBSM_FORCE_32_BIT") {
        return static_cast<int>(EMBSM_FORCE_32_BIT);
    }
    if (s == "EMBSM_LEFT") {
        return static_cast<int>(EMBSM_LEFT);
    }
    if (s == "EMBSM_MIDDLE") {
        return static_cast<int>(EMBSM_MIDDLE);
    }
    if (s == "EMBSM_RIGHT") {
        return static_cast<int>(EMBSM_RIGHT);
    }
    if (s == "EMD3_ANIMATION_COUNT") {
        return static_cast<int>(EMD3_ANIMATION_COUNT);
    }
    if (s == "EMD3_BOTH_DEAD_1") {
        return static_cast<int>(EMD3_BOTH_DEAD_1);
    }
    if (s == "EMD3_BOTH_DEAD_2") {
        return static_cast<int>(EMD3_BOTH_DEAD_2);
    }
    if (s == "EMD3_BOTH_DEAD_3") {
        return static_cast<int>(EMD3_BOTH_DEAD_3);
    }
    if (s == "EMD3_BOTH_DEATH_1") {
        return static_cast<int>(EMD3_BOTH_DEATH_1);
    }
    if (s == "EMD3_BOTH_DEATH_2") {
        return static_cast<int>(EMD3_BOTH_DEATH_2);
    }
    if (s == "EMD3_BOTH_DEATH_3") {
        return static_cast<int>(EMD3_BOTH_DEATH_3);
    }
    if (s == "EMD3_LEGS_BACK") {
        return static_cast<int>(EMD3_LEGS_BACK);
    }
    if (s == "EMD3_LEGS_IDLE_CROUCH") {
        return static_cast<int>(EMD3_LEGS_IDLE_CROUCH);
    }
    if (s == "EMD3_LEGS_IDLE") {
        return static_cast<int>(EMD3_LEGS_IDLE);
    }
    if (s == "EMD3_LEGS_JUMP_1") {
        return static_cast<int>(EMD3_LEGS_JUMP_1);
    }
    if (s == "EMD3_LEGS_JUMP_2") {
        return static_cast<int>(EMD3_LEGS_JUMP_2);
    }
    if (s == "EMD3_LEGS_LAND_1") {
        return static_cast<int>(EMD3_LEGS_LAND_1);
    }
    if (s == "EMD3_LEGS_LAND_2") {
        return static_cast<int>(EMD3_LEGS_LAND_2);
    }
    if (s == "EMD3_LEGS_RUN") {
        return static_cast<int>(EMD3_LEGS_RUN);
    }
    if (s == "EMD3_LEGS_SWIM") {
        return static_cast<int>(EMD3_LEGS_SWIM);
    }
    if (s == "EMD3_LEGS_TURN") {
        return static_cast<int>(EMD3_LEGS_TURN);
    }
    if (s == "EMD3_LEGS_WALK_CROUCH") {
        return static_cast<int>(EMD3_LEGS_WALK_CROUCH);
    }
    if (s == "EMD3_LEGS_WALK") {
        return static_cast<int>(EMD3_LEGS_WALK);
    }
    if (s == "EMD3_TORSO_ATTACK_1") {
        return static_cast<int>(EMD3_TORSO_ATTACK_1);
    }
    if (s == "EMD3_TORSO_ATTACK_2") {
        return static_cast<int>(EMD3_TORSO_ATTACK_2);
    }
    if (s == "EMD3_TORSO_DROP") {
        return static_cast<int>(EMD3_TORSO_DROP);
    }
    if (s == "EMD3_TORSO_GESTURE") {
        return static_cast<int>(EMD3_TORSO_GESTURE);
    }
    if (s == "EMD3_TORSO_RAISE") {
        return static_cast<int>(EMD3_TORSO_RAISE);
    }
    if (s == "EMD3_TORSO_STAND_1") {
        return static_cast<int>(EMD3_TORSO_STAND_1);
    }
    if (s == "EMD3_TORSO_STAND_2") {
        return static_cast<int>(EMD3_TORSO_STAND_2);
    }
    if (s == "EMF_ANISOTROPIC_FILTER") {
        return static_cast<int>(EMF_ANISOTROPIC_FILTER);
    }
    if (s == "EMF_ANTI_ALIASING") {
        return static_cast<int>(EMF_ANTI_ALIASING);
    }
    if (s == "EMF_BACK_FACE_CULLING") {
        return static_cast<int>(EMF_BACK_FACE_CULLING);
    }
    if (s == "EMF_BILINEAR_FILTER") {
        return static_cast<int>(EMF_BILINEAR_FILTER);
    }
    if (s == "EMF_BLEND_OPERATION") {
        return static_cast<int>(EMF_BLEND_OPERATION);
    }
    if (s == "EMF_COLOR_MASK") {
        return static_cast<int>(EMF_COLOR_MASK);
    }
    if (s == "EMF_COLOR_MATERIAL") {
        return static_cast<int>(EMF_COLOR_MATERIAL);
    }
    if (s == "EMF_FOG_ENABLE") {
        return static_cast<int>(EMF_FOG_ENABLE);
    }
    if (s == "EMF_FRONT_FACE_CULLING") {
        return static_cast<int>(EMF_FRONT_FACE_CULLING);
    }
    if (s == "EMF_GOURAUD_SHADING") {
        return static_cast<int>(EMF_GOURAUD_SHADING);
    }
    if (s == "EMF_LIGHTING") {
        return static_cast<int>(EMF_LIGHTING);
    }
    if (s == "EMFN_MODULATE_1X") {
        return static_cast<int>(EMFN_MODULATE_1X);
    }
    if (s == "EMFN_MODULATE_2X") {
        return static_cast<int>(EMFN_MODULATE_2X);
    }
    if (s == "EMFN_MODULATE_4X") {
        return static_cast<int>(EMFN_MODULATE_4X);
    }
    if (s == "EMF_NORMALIZE_NORMALS") {
        return static_cast<int>(EMF_NORMALIZE_NORMALS);
    }
    if (s == "EMF_POINTCLOUD") {
        return static_cast<int>(EMF_POINTCLOUD);
    }
    if (s == "EMF_POLYGON_OFFSET") {
        return static_cast<int>(EMF_POLYGON_OFFSET);
    }
    if (s == "EMF_TEXTURE_WRAP") {
        return static_cast<int>(EMF_TEXTURE_WRAP);
    }
    if (s == "EMF_TRILINEAR_FILTER") {
        return static_cast<int>(EMF_TRILINEAR_FILTER);
    }
    if (s == "EMF_USE_MIP_MAPS") {
        return static_cast<int>(EMF_USE_MIP_MAPS);
    }
    if (s == "EMF_WIREFRAME") {
        return static_cast<int>(EMF_WIREFRAME);
    }
    if (s == "EMF_ZBUFFER") {
        return static_cast<int>(EMF_ZBUFFER);
    }
    if (s == "EMF_ZWRITE_ENABLE") {
        return static_cast<int>(EMF_ZWRITE_ENABLE);
    }
    if (s == "EMIE_COUNT") {
        return static_cast<int>(EMIE_COUNT);
    }
    if (s == "EMIE_LMOUSE_DOUBLE_CLICK") {
        return static_cast<int>(EMIE_LMOUSE_DOUBLE_CLICK);
    }
    if (s == "EMIE_LMOUSE_LEFT_UP") {
        return static_cast<int>(EMIE_LMOUSE_LEFT_UP);
    }
    if (s == "EMIE_LMOUSE_PRESSED_DOWN") {
        return static_cast<int>(EMIE_LMOUSE_PRESSED_DOWN);
    }
    if (s == "EMIE_LMOUSE_TRIPLE_CLICK") {
        return static_cast<int>(EMIE_LMOUSE_TRIPLE_CLICK);
    }
    if (s == "EMIE_MMOUSE_DOUBLE_CLICK") {
        return static_cast<int>(EMIE_MMOUSE_DOUBLE_CLICK);
    }
    if (s == "EMIE_MMOUSE_LEFT_UP") {
        return static_cast<int>(EMIE_MMOUSE_LEFT_UP);
    }
    if (s == "EMIE_MMOUSE_PRESSED_DOWN") {
        return static_cast<int>(EMIE_MMOUSE_PRESSED_DOWN);
    }
    if (s == "EMIE_MMOUSE_TRIPLE_CLICK") {
        return static_cast<int>(EMIE_MMOUSE_TRIPLE_CLICK);
    }
    if (s == "EMIE_MOUSE_MOVED") {
        return static_cast<int>(EMIE_MOUSE_MOVED);
    }
    if (s == "EMIE_MOUSE_WHEEL") {
        return static_cast<int>(EMIE_MOUSE_WHEEL);
    }
    if (s == "EMIE_RMOUSE_DOUBLE_CLICK") {
        return static_cast<int>(EMIE_RMOUSE_DOUBLE_CLICK);
    }
    if (s == "EMIE_RMOUSE_LEFT_UP") {
        return static_cast<int>(EMIE_RMOUSE_LEFT_UP);
    }
    if (s == "EMIE_RMOUSE_PRESSED_DOWN") {
        return static_cast<int>(EMIE_RMOUSE_PRESSED_DOWN);
    }
    if (s == "EMIE_RMOUSE_TRIPLE_CLICK") {
        return static_cast<int>(EMIE_RMOUSE_TRIPLE_CLICK);
    }
    if (s == "EMT_DETAIL_MAP") {
        return static_cast<int>(EMT_DETAIL_MAP);
    }
    if (s == "EMT_FORCE_32BIT") {
        return static_cast<int>(EMT_FORCE_32BIT);
    }
    if (s == "EMT_LIGHTMAP_ADD") {
        return static_cast<int>(EMT_LIGHTMAP_ADD);
    }
    if (s == "EMT_LIGHTMAP_LIGHTING_M2") {
        return static_cast<int>(EMT_LIGHTMAP_LIGHTING_M2);
    }
    if (s == "EMT_LIGHTMAP_LIGHTING_M4") {
        return static_cast<int>(EMT_LIGHTMAP_LIGHTING_M4);
    }
    if (s == "EMT_LIGHTMAP_LIGHTING") {
        return static_cast<int>(EMT_LIGHTMAP_LIGHTING);
    }
    if (s == "EMT_LIGHTMAP_M2") {
        return static_cast<int>(EMT_LIGHTMAP_M2);
    }
    if (s == "EMT_LIGHTMAP_M4") {
        return static_cast<int>(EMT_LIGHTMAP_M4);
    }
    if (s == "EMT_LIGHTMAP") {
        return static_cast<int>(EMT_LIGHTMAP);
    }
    if (s == "EMT_NORMAL_MAP_SOLID") {
        return static_cast<int>(EMT_NORMAL_MAP_SOLID);
    }
    if (s == "EMT_NORMAL_MAP_TRANSPARENT_ADD_COLOR") {
        return static_cast<int>(EMT_NORMAL_MAP_TRANSPARENT_ADD_COLOR);
    }
    if (s == "EMT_NORMAL_MAP_TRANSPARENT_VERTEX_ALPHA") {
        return static_cast<int>(EMT_NORMAL_MAP_TRANSPARENT_VERTEX_ALPHA);
    }
    if (s == "EMT_ONETEXTURE_BLEND") {
        return static_cast<int>(EMT_ONETEXTURE_BLEND);
    }
    if (s == "EMT_PARALLAX_MAP_SOLID") {
        return static_cast<int>(EMT_PARALLAX_MAP_SOLID);
    }
    if (s == "EMT_PARALLAX_MAP_TRANSPARENT_ADD_COLOR") {
        return static_cast<int>(EMT_PARALLAX_MAP_TRANSPARENT_ADD_COLOR);
    }
    if (s == "EMT_PARALLAX_MAP_TRANSPARENT_VERTEX_ALPHA") {
        return static_cast<int>(EMT_PARALLAX_MAP_TRANSPARENT_VERTEX_ALPHA);
    }
    if (s == "EMT_REFLECTION_2_LAYER") {
        return static_cast<int>(EMT_REFLECTION_2_LAYER);
    }
    if (s == "EMT_SOLID_2_LAYER") {
        return static_cast<int>(EMT_SOLID_2_LAYER);
    }
    if (s == "EMT_SOLID") {
        return static_cast<int>(EMT_SOLID);
    }
    if (s == "EMT_SPHERE_MAP") {
        return static_cast<int>(EMT_SPHERE_MAP);
    }
    if (s == "EMT_TRANSPARENT_ADD_COLOR") {
        return static_cast<int>(EMT_TRANSPARENT_ADD_COLOR);
    }
    if (s == "EMT_TRANSPARENT_ALPHA_CHANNEL_REF") {
        return static_cast<int>(EMT_TRANSPARENT_ALPHA_CHANNEL_REF);
    }
    if (s == "EMT_TRANSPARENT_ALPHA_CHANNEL") {
        return static_cast<int>(EMT_TRANSPARENT_ALPHA_CHANNEL);
    }
    if (s == "EMT_TRANSPARENT_REFLECTION_2_LAYER") {
        return static_cast<int>(EMT_TRANSPARENT_REFLECTION_2_LAYER);
    }
    if (s == "EMT_TRANSPARENT_VERTEX_ALPHA") {
        return static_cast<int>(EMT_TRANSPARENT_VERTEX_ALPHA);
    }
    if (s == "EMWF_NONE") {
        return static_cast<int>(EMWF_NONE);
    }
    if (s == "EMWF_WRITE_BINARY") {
        return static_cast<int>(EMWF_WRITE_BINARY);
    }
    if (s == "EMWF_WRITE_COMPRESSED") {
        return static_cast<int>(EMWF_WRITE_COMPRESSED);
    }
    if (s == "EMWF_WRITE_LIGHTMAPS") {
        return static_cast<int>(EMWF_WRITE_LIGHTMAPS);
    }
    if (s == "EMWT_COLLADA") {
        return static_cast<int>(EMWT_COLLADA);
    }
    if (s == "EMWT_IRR_MESH") {
        return static_cast<int>(EMWT_IRR_MESH);
    }
    if (s == "EMWT_OBJ") {
        return static_cast<int>(EMWT_OBJ);
    }
    if (s == "EMWT_PLY") {
        return static_cast<int>(EMWT_PLY);
    }
    if (s == "EMWT_STL") {
        return static_cast<int>(EMWT_STL);
    }
    if (s == "EPAT_ATTRACT") {
        return static_cast<int>(EPAT_ATTRACT);
    }
    if (s == "EPAT_COUNT") {
        return static_cast<int>(EPAT_COUNT);
    }
    if (s == "EPAT_FADE_OUT") {
        return static_cast<int>(EPAT_FADE_OUT);
    }
    if (s == "EPAT_GRAVITY") {
        return static_cast<int>(EPAT_GRAVITY);
    }
    if (s == "EPAT_NONE") {
        return static_cast<int>(EPAT_NONE);
    }
    if (s == "EPAT_ROTATE") {
        return static_cast<int>(EPAT_ROTATE);
    }
    if (s == "EPAT_SCALE") {
        return static_cast<int>(EPAT_SCALE);
    }
    if (s == "EPET_ANIMATED_MESH") {
        return static_cast<int>(EPET_ANIMATED_MESH);
    }
    if (s == "EPET_BOX") {
        return static_cast<int>(EPET_BOX);
    }
    if (s == "EPET_COUNT") {
        return static_cast<int>(EPET_COUNT);
    }
    if (s == "EPET_CYLINDER") {
        return static_cast<int>(EPET_CYLINDER);
    }
    if (s == "EPET_MESH") {
        return static_cast<int>(EPET_MESH);
    }
    if (s == "EPET_POINT") {
        return static_cast<int>(EPET_POINT);
    }
    if (s == "EPET_RING") {
        return static_cast<int>(EPET_RING);
    }
    if (s == "EPET_SPHERE") {
        return static_cast<int>(EPET_SPHERE);
    }
    if (s == "EPO_BACK") {
        return static_cast<int>(EPO_BACK);
    }
    if (s == "EPO_FRONT") {
        return static_cast<int>(EPO_FRONT);
    }
    if (s == "EPST_COUNT") {
        return static_cast<int>(EPST_COUNT);
    }
    if (s == "EPST_PS_1_1") {
        return static_cast<int>(EPST_PS_1_1);
    }
    if (s == "EPST_PS_1_2") {
        return static_cast<int>(EPST_PS_1_2);
    }
    if (s == "EPST_PS_1_3") {
        return static_cast<int>(EPST_PS_1_3);
    }
    if (s == "EPST_PS_1_4") {
        return static_cast<int>(EPST_PS_1_4);
    }
    if (s == "EPST_PS_2_0") {
        return static_cast<int>(EPST_PS_2_0);
    }
    if (s == "EPST_PS_3_0") {
        return static_cast<int>(EPST_PS_3_0);
    }
    if (s == "EPST_PS_4_0") {
        return static_cast<int>(EPST_PS_4_0);
    }
    if (s == "EPST_PS_4_1") {
        return static_cast<int>(EPST_PS_4_1);
    }
    if (s == "EPST_PS_5_0") {
        return static_cast<int>(EPST_PS_5_0);
    }
    if (s == "EPT_LINE_LOOP") {
        return static_cast<int>(EPT_LINE_LOOP);
    }
    if (s == "EPT_LINES") {
        return static_cast<int>(EPT_LINES);
    }
    if (s == "EPT_LINE_STRIP") {
        return static_cast<int>(EPT_LINE_STRIP);
    }
    if (s == "EPT_POINT_SPRITES") {
        return static_cast<int>(EPT_POINT_SPRITES);
    }
    if (s == "EPT_POINTS") {
        return static_cast<int>(EPT_POINTS);
    }
    if (s == "EPT_POLYGON") {
        return static_cast<int>(EPT_POLYGON);
    }
    if (s == "EPT_QUADS") {
        return static_cast<int>(EPT_QUADS);
    }
    if (s == "EPT_QUAD_STRIP") {
        return static_cast<int>(EPT_QUAD_STRIP);
    }
    if (s == "EPT_TRIANGLE_FAN") {
        return static_cast<int>(EPT_TRIANGLE_FAN);
    }
    if (s == "EPT_TRIANGLES") {
        return static_cast<int>(EPT_TRIANGLES);
    }
    if (s == "EPT_TRIANGLE_STRIP") {
        return static_cast<int>(EPT_TRIANGLE_STRIP);
    }
    if (s == "ERT_AUX_BUFFER0") {
        return static_cast<int>(ERT_AUX_BUFFER0);
    }
    if (s == "ERT_AUX_BUFFER1") {
        return static_cast<int>(ERT_AUX_BUFFER1);
    }
    if (s == "ERT_AUX_BUFFER2") {
        return static_cast<int>(ERT_AUX_BUFFER2);
    }
    if (s == "ERT_AUX_BUFFER3") {
        return static_cast<int>(ERT_AUX_BUFFER3);
    }
    if (s == "ERT_AUX_BUFFER4") {
        return static_cast<int>(ERT_AUX_BUFFER4);
    }
    if (s == "ERT_FRAME_BUFFER") {
        return static_cast<int>(ERT_FRAME_BUFFER);
    }
    if (s == "ERT_MULTI_RENDER_TEXTURES") {
        return static_cast<int>(ERT_MULTI_RENDER_TEXTURES);
    }
    if (s == "ERT_RENDER_TEXTURE") {
        return static_cast<int>(ERT_RENDER_TEXTURE);
    }
    if (s == "ERT_STEREO_BOTH_BUFFERS") {
        return static_cast<int>(ERT_STEREO_BOTH_BUFFERS);
    }
    if (s == "ERT_STEREO_LEFT_BUFFER") {
        return static_cast<int>(ERT_STEREO_LEFT_BUFFER);
    }
    if (s == "ERT_STEREO_RIGHT_BUFFER") {
        return static_cast<int>(ERT_STEREO_RIGHT_BUFFER);
    }
    if (s == "ESNAT_CAMERA_FPS") {
        return static_cast<int>(ESNAT_CAMERA_FPS);
    }
    if (s == "ESNAT_CAMERA_MAYA") {
        return static_cast<int>(ESNAT_CAMERA_MAYA);
    }
    if (s == "ESNAT_COLLISION_RESPONSE") {
        return static_cast<int>(ESNAT_COLLISION_RESPONSE);
    }
    if (s == "ESNAT_COUNT") {
        return static_cast<int>(ESNAT_COUNT);
    }
    if (s == "ESNAT_DELETION") {
        return static_cast<int>(ESNAT_DELETION);
    }
    if (s == "ESNAT_FLY_CIRCLE") {
        return static_cast<int>(ESNAT_FLY_CIRCLE);
    }
    if (s == "ESNAT_FLY_STRAIGHT") {
        return static_cast<int>(ESNAT_FLY_STRAIGHT);
    }
    if (s == "ESNAT_FOLLOW_SPLINE") {
        return static_cast<int>(ESNAT_FOLLOW_SPLINE);
    }
    if (s == "ESNAT_FORCE_32_BIT") {
        return static_cast<int>(ESNAT_FORCE_32_BIT);
    }
    if (s == "ESNAT_ROTATION") {
        return static_cast<int>(ESNAT_ROTATION);
    }
    if (s == "ESNAT_TEXTURE") {
        return static_cast<int>(ESNAT_TEXTURE);
    }
    if (s == "ESNAT_UNKNOWN") {
        return static_cast<int>(ESNAT_UNKNOWN);
    }
    if (s == "ESNRP_AUTOMATIC") {
        return static_cast<int>(ESNRP_AUTOMATIC);
    }
    if (s == "ESNRP_CAMERA") {
        return static_cast<int>(ESNRP_CAMERA);
    }
    if (s == "ESNRP_LIGHT") {
        return static_cast<int>(ESNRP_LIGHT);
    }
    if (s == "ESNRP_NONE") {
        return static_cast<int>(ESNRP_NONE);
    }
    if (s == "ESNRP_SHADOW") {
        return static_cast<int>(ESNRP_SHADOW);
    }
    if (s == "ESNRP_SKY_BOX") {
        return static_cast<int>(ESNRP_SKY_BOX);
    }
    if (s == "ESNRP_SOLID") {
        return static_cast<int>(ESNRP_SOLID);
    }
    if (s == "ESNRP_TRANSPARENT_EFFECT") {
        return static_cast<int>(ESNRP_TRANSPARENT_EFFECT);
    }
    if (s == "ESNRP_TRANSPARENT") {
        return static_cast<int>(ESNRP_TRANSPARENT);
    }
    if (s == "ESNT_ANIMATED_MESH") {
        return static_cast<int>(ESNT_ANIMATED_MESH);
    }
    if (s == "ESNT_ANY") {
        return static_cast<int>(ESNT_ANY);
    }
    if (s == "ESNT_BILLBOARD") {
        return static_cast<int>(ESNT_BILLBOARD);
    }
    if (s == "ESNT_CAMERA_FPS") {
        return static_cast<int>(ESNT_CAMERA_FPS);
    }
    if (s == "ESNT_CAMERA_MAYA") {
        return static_cast<int>(ESNT_CAMERA_MAYA);
    }
    if (s == "ESNT_CAMERA") {
        return static_cast<int>(ESNT_CAMERA);
    }
    if (s == "ESNT_CUBE") {
        return static_cast<int>(ESNT_CUBE);
    }
    if (s == "ESNT_DUMMY_TRANSFORMATION") {
        return static_cast<int>(ESNT_DUMMY_TRANSFORMATION);
    }
    if (s == "ESNT_EMPTY") {
        return static_cast<int>(ESNT_EMPTY);
    }
    if (s == "ESNT_LIGHT") {
        return static_cast<int>(ESNT_LIGHT);
    }
    if (s == "ESNT_MD3_SCENE_NODE") {
        return static_cast<int>(ESNT_MD3_SCENE_NODE);
    }
    if (s == "ESNT_MESH") {
        return static_cast<int>(ESNT_MESH);
    }
    if (s == "ESNT_OCTREE") {
        return static_cast<int>(ESNT_OCTREE);
    }
    if (s == "ESNT_PARTICLE_SYSTEM") {
        return static_cast<int>(ESNT_PARTICLE_SYSTEM);
    }
    if (s == "ESNT_Q3SHADER_SCENE_NODE") {
        return static_cast<int>(ESNT_Q3SHADER_SCENE_NODE);
    }
    if (s == "ESNT_SCENE_MANAGER") {
        return static_cast<int>(ESNT_SCENE_MANAGER);
    }
    if (s == "ESNT_SHADOW_VOLUME") {
        return static_cast<int>(ESNT_SHADOW_VOLUME);
    }
    if (s == "ESNT_SKY_BOX") {
        return static_cast<int>(ESNT_SKY_BOX);
    }
    if (s == "ESNT_SKY_DOME") {
        return static_cast<int>(ESNT_SKY_DOME);
    }
    if (s == "ESNT_SPHERE") {
        return static_cast<int>(ESNT_SPHERE);
    }
    if (s == "ESNT_TERRAIN") {
        return static_cast<int>(ESNT_TERRAIN);
    }
    if (s == "ESNT_TEXT") {
        return static_cast<int>(ESNT_TEXT);
    }
    if (s == "ESNT_UNKNOWN") {
        return static_cast<int>(ESNT_UNKNOWN);
    }
    if (s == "ESNT_VOLUME_LIGHT") {
        return static_cast<int>(ESNT_VOLUME_LIGHT);
    }
    if (s == "ESNT_WATER_SURFACE") {
        return static_cast<int>(ESNT_WATER_SURFACE);
    }
    if (s == "ETC_CLAMP") {
        return static_cast<int>(ETC_CLAMP);
    }
    if (s == "ETC_CLAMP_TO_BORDER") {
        return static_cast<int>(ETC_CLAMP_TO_BORDER);
    }
    if (s == "ETC_CLAMP_TO_EDGE") {
        return static_cast<int>(ETC_CLAMP_TO_EDGE);
    }
    if (s == "ETCF_ALLOW_NON_POWER_2") {
        return static_cast<int>(ETCF_ALLOW_NON_POWER_2);
    }
    if (s == "ETCF_ALWAYS_16_BIT") {
        return static_cast<int>(ETCF_ALWAYS_16_BIT);
    }
    if (s == "ETCF_ALWAYS_32_BIT") {
        return static_cast<int>(ETCF_ALWAYS_32_BIT);
    }
    if (s == "ETCF_CREATE_MIP_MAPS") {
        return static_cast<int>(ETCF_CREATE_MIP_MAPS);
    }
    if (s == "ETCF_FORCE_32_BIT_DO_NOT_USE") {
        return static_cast<int>(ETCF_FORCE_32_BIT_DO_NOT_USE);
    }
    if (s == "ETCF_NO_ALPHA_CHANNEL") {
        return static_cast<int>(ETCF_NO_ALPHA_CHANNEL);
    }
    if (s == "ETCF_OPTIMIZED_FOR_QUALITY") {
        return static_cast<int>(ETCF_OPTIMIZED_FOR_QUALITY);
    }
    if (s == "ETCF_OPTIMIZED_FOR_SPEED") {
        return static_cast<int>(ETCF_OPTIMIZED_FOR_SPEED);
    }
    if (s == "ETC_MIRROR_CLAMP") {
        return static_cast<int>(ETC_MIRROR_CLAMP);
    }
    if (s == "ETC_MIRROR_CLAMP_TO_BORDER") {
        return static_cast<int>(ETC_MIRROR_CLAMP_TO_BORDER);
    }
    if (s == "ETC_MIRROR_CLAMP_TO_EDGE") {
        return static_cast<int>(ETC_MIRROR_CLAMP_TO_EDGE);
    }
    if (s == "ETC_MIRROR") {
        return static_cast<int>(ETC_MIRROR);
    }
    if (s == "ETC_REPEAT") {
        return static_cast<int>(ETC_REPEAT);
    }
    if (s == "ETF_ASCII") {
        return static_cast<int>(ETF_ASCII);
    }
    if (s == "ETF_UTF16_BE") {
        return static_cast<int>(ETF_UTF16_BE);
    }
    if (s == "ETF_UTF16_LE") {
        return static_cast<int>(ETF_UTF16_LE);
    }
    if (s == "ETF_UTF32_BE") {
        return static_cast<int>(ETF_UTF32_BE);
    }
    if (s == "ETF_UTF32_LE") {
        return static_cast<int>(ETF_UTF32_LE);
    }
    if (s == "ETF_UTF8") {
        return static_cast<int>(ETF_UTF8);
    }
    if (s == "ETLM_READ_ONLY") {
        return static_cast<int>(ETLM_READ_ONLY);
    }
    if (s == "ETLM_READ_WRITE") {
        return static_cast<int>(ETLM_READ_WRITE);
    }
    if (s == "ETLM_WRITE_ONLY") {
        return static_cast<int>(ETLM_WRITE_ONLY);
    }
    if (s == "ETPS_129") {
        return static_cast<int>(ETPS_129);
    }
    if (s == "ETPS_17") {
        return static_cast<int>(ETPS_17);
    }
    if (s == "ETPS_33") {
        return static_cast<int>(ETPS_33);
    }
    if (s == "ETPS_65") {
        return static_cast<int>(ETPS_65);
    }
    if (s == "ETPS_9") {
        return static_cast<int>(ETPS_9);
    }
    if (s == "ETS_COUNT") {
        return static_cast<int>(ETS_COUNT);
    }
    if (s == "ETS_PROJECTION") {
        return static_cast<int>(ETS_PROJECTION);
    }
    if (s == "ETS_TEXTURE_0") {
        return static_cast<int>(ETS_TEXTURE_0);
    }
    if (s == "ETS_TEXTURE_1") {
        return static_cast<int>(ETS_TEXTURE_1);
    }
    if (s == "ETS_TEXTURE_2") {
        return static_cast<int>(ETS_TEXTURE_2);
    }
    if (s == "ETS_TEXTURE_3") {
        return static_cast<int>(ETS_TEXTURE_3);
    }
    if (s == "ETS_VIEW") {
        return static_cast<int>(ETS_VIEW);
    }
    if (s == "ETS_WORLD") {
        return static_cast<int>(ETS_WORLD);
    }
    if (s == "EVDF_ALPHA_TO_COVERAGE") {
        return static_cast<int>(EVDF_ALPHA_TO_COVERAGE);
    }
    if (s == "EVDF_ARB_FRAGMENT_PROGRAM_1") {
        return static_cast<int>(EVDF_ARB_FRAGMENT_PROGRAM_1);
    }
    if (s == "EVDF_ARB_GLSL") {
        return static_cast<int>(EVDF_ARB_GLSL);
    }
    if (s == "EVDF_ARB_VERTEX_PROGRAM_1") {
        return static_cast<int>(EVDF_ARB_VERTEX_PROGRAM_1);
    }
    if (s == "EVDF_BILINEAR_FILTER") {
        return static_cast<int>(EVDF_BILINEAR_FILTER);
    }
    if (s == "EVDF_BLEND_OPERATIONS") {
        return static_cast<int>(EVDF_BLEND_OPERATIONS);
    }
    if (s == "EVDF_COLOR_MASK") {
        return static_cast<int>(EVDF_COLOR_MASK);
    }
    if (s == "EVDF_COUNT") {
        return static_cast<int>(EVDF_COUNT);
    }
    if (s == "EVDF_FRAMEBUFFER_OBJECT") {
        return static_cast<int>(EVDF_FRAMEBUFFER_OBJECT);
    }
    if (s == "EVDF_GEOMETRY_SHADER") {
        return static_cast<int>(EVDF_GEOMETRY_SHADER);
    }
    if (s == "EVDF_HARDWARE_TL") {
        return static_cast<int>(EVDF_HARDWARE_TL);
    }
    if (s == "EVDF_HLSL") {
        return static_cast<int>(EVDF_HLSL);
    }
    if (s == "EVDF_MIP_MAP_AUTO_UPDATE") {
        return static_cast<int>(EVDF_MIP_MAP_AUTO_UPDATE);
    }
    if (s == "EVDF_MIP_MAP") {
        return static_cast<int>(EVDF_MIP_MAP);
    }
    if (s == "EVDF_MRT_BLEND_FUNC") {
        return static_cast<int>(EVDF_MRT_BLEND_FUNC);
    }
    if (s == "EVDF_MRT_BLEND") {
        return static_cast<int>(EVDF_MRT_BLEND);
    }
    if (s == "EVDF_MRT_COLOR_MASK") {
        return static_cast<int>(EVDF_MRT_COLOR_MASK);
    }
    if (s == "EVDF_MULTIPLE_RENDER_TARGETS") {
        return static_cast<int>(EVDF_MULTIPLE_RENDER_TARGETS);
    }
    if (s == "EVDF_MULTITEXTURE") {
        return static_cast<int>(EVDF_MULTITEXTURE);
    }
    if (s == "EVDF_OCCLUSION_QUERY") {
        return static_cast<int>(EVDF_OCCLUSION_QUERY);
    }
    if (s == "EVDF_PIXEL_SHADER_1_1") {
        return static_cast<int>(EVDF_PIXEL_SHADER_1_1);
    }
    if (s == "EVDF_PIXEL_SHADER_1_2") {
        return static_cast<int>(EVDF_PIXEL_SHADER_1_2);
    }
    if (s == "EVDF_PIXEL_SHADER_1_3") {
        return static_cast<int>(EVDF_PIXEL_SHADER_1_3);
    }
    if (s == "EVDF_PIXEL_SHADER_1_4") {
        return static_cast<int>(EVDF_PIXEL_SHADER_1_4);
    }
    if (s == "EVDF_PIXEL_SHADER_2_0") {
        return static_cast<int>(EVDF_PIXEL_SHADER_2_0);
    }
    if (s == "EVDF_PIXEL_SHADER_3_0") {
        return static_cast<int>(EVDF_PIXEL_SHADER_3_0);
    }
    if (s == "EVDF_POLYGON_OFFSET") {
        return static_cast<int>(EVDF_POLYGON_OFFSET);
    }
    if (s == "EVDF_RENDER_TO_TARGET") {
        return static_cast<int>(EVDF_RENDER_TO_TARGET);
    }
    if (s == "EVDF_STENCIL_BUFFER") {
        return static_cast<int>(EVDF_STENCIL_BUFFER);
    }
    if (s == "EVDF_TEXTURE_MATRIX") {
        return static_cast<int>(EVDF_TEXTURE_MATRIX);
    }
    if (s == "EVDF_TEXTURE_NPOT") {
        return static_cast<int>(EVDF_TEXTURE_NPOT);
    }
    if (s == "EVDF_TEXTURE_NSQUARE") {
        return static_cast<int>(EVDF_TEXTURE_NSQUARE);
    }
    if (s == "EVDF_VERTEX_BUFFER_OBJECT") {
        return static_cast<int>(EVDF_VERTEX_BUFFER_OBJECT);
    }
    if (s == "EVDF_VERTEX_SHADER_1_1") {
        return static_cast<int>(EVDF_VERTEX_SHADER_1_1);
    }
    if (s == "EVDF_VERTEX_SHADER_2_0") {
        return static_cast<int>(EVDF_VERTEX_SHADER_2_0);
    }
    if (s == "EVDF_VERTEX_SHADER_3_0") {
        return static_cast<int>(EVDF_VERTEX_SHADER_3_0);
    }
    if (s == "EVST_COUNT") {
        return static_cast<int>(EVST_COUNT);
    }
    if (s == "EVST_VS_1_1") {
        return static_cast<int>(EVST_VS_1_1);
    }
    if (s == "EVST_VS_2_0") {
        return static_cast<int>(EVST_VS_2_0);
    }
    if (s == "EVST_VS_3_0") {
        return static_cast<int>(EVST_VS_3_0);
    }
    if (s == "EVST_VS_4_0") {
        return static_cast<int>(EVST_VS_4_0);
    }
    if (s == "EVST_VS_4_1") {
        return static_cast<int>(EVST_VS_4_1);
    }
    if (s == "EVST_VS_5_0") {
        return static_cast<int>(EVST_VS_5_0);
    }
    if (s == "EVT_2TCOORDS") {
        return static_cast<int>(EVT_2TCOORDS);
    }
    if (s == "EVT_STANDARD") {
        return static_cast<int>(EVT_STANDARD);
    }
    if (s == "EVT_TANGENTS") {
        return static_cast<int>(EVT_TANGENTS);
    }
    if (s == "EXN_CDATA") {
        return static_cast<int>(EXN_CDATA);
    }
    if (s == "EXN_COMMENT") {
        return static_cast<int>(EXN_COMMENT);
    }
    if (s == "EXN_ELEMENT_END") {
        return static_cast<int>(EXN_ELEMENT_END);
    }
    if (s == "EXN_ELEMENT") {
        return static_cast<int>(EXN_ELEMENT);
    }
    if (s == "EXN_NONE") {
        return static_cast<int>(EXN_NONE);
    }
    if (s == "EXN_TEXT") {
        return static_cast<int>(EXN_TEXT);
    }
    if (s == "EXN_UNKNOWN") {
        return static_cast<int>(EXN_UNKNOWN);
    }
    throw std::runtime_error("Unknown Irrlicht enum value: '" + s + "'");
}
