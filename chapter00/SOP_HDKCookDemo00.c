#include "SOP_HDKCookDemo00.h"

#include <GU/GU_Detail.h>
#include <GEO/GEO_PrimPoly.h>
#include <GEO/GEO_AttributeHandle.h>
#include <OP/OP_Operator.h>
#include <OP/OP_OperatorTable.h>
#include <PRM/PRM_Include.h>
#include <PRM/PRM_TemplateBuilder.h>
#include <UT/UT_DSOVersion.h>
#include <UT/UT_Interrupt.h>
#include <UT/UT_StringHolder.h>
#include <SYS/SYS_Math.h>
#include <limits.h>

using namespace HDK_CookDemo;

static PRM_Name names[] = {
    PRM_Name("shape", "Shape"),
    PRM_Name("scale", "Scale"),
    PRM_Name("depthscale","Depth scale"),
    PRM_Name("depth", "Depth"),
    PRM_Name("detail", "Detail"),
    PRM_Name("usetgt", "Use Target pos"),
    PRM_Name("tgtpos", "Target pos")
};

static PRM_Name shapes[] = {
    PRM_Name("box", "Box"),
    PRM_Name("pyramid", "Pyramid"),
    PRM_Name("diamond", "Diamond"),
    PRM_Name()
};

static PRM_Default defaultDepthScale(0.75f);
static PRM_Range detailDepthRange(PRM_RANGE_UI, 1, PRM_RANGE_UI, 7);
static PRM_ChoiceList shapeChoiceList(PRM_CHOICELIST_SINGLE, shapes);
const UT_StringHolder SOP_HDKCookDemo00::theSOPTypeName("hdkcookdemo00");

void newSopOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator(
        SOP_HDKCookDemo00::theSOPTypeName,
        "SOP_HDKCookDemo00",
        SOP_HDKCookDemo00::myConstructor,
        SOP_HDKCookDemo00::parmTemplateList,
        0,
        0,
        nullptr,
        OP_FLAG_GENERATOR));
}

PRM_Template SOP_HDKCookDemo00::parmTemplateList[] = {
    PRM_Template(PRM_ORD, 1, &names[0], PRMzeroDefaults, &shapeChoiceList),
    PRM_Template(PRM_XYZ, 3, &names[1], PRMoneDefaults),
    PRM_Template(PRM_FLT, 1, &names[2], &defaultDepthScale, 0),
    PRM_Template(PRM_INT, 1, &names[3], PRMoneDefaults, 0, &detailDepthRange),
    PRM_Template(PRM_INT, 1, &names[4], PRMoneDefaults, 0, &detailDepthRange),
    PRM_Template(PRM_TOGGLE, 1, &names[5]),
    PRM_Template(PRM_XYZ, 3, &names[6], PRMzeroDefaults),
    PRM_Template(),
};

OP_ERROR SOP_HDKCookDemo00::cookDemo(OP_Context& context)
{
    gdp->clearAndDestroy();

    fpreal now = context.getTime();
    int offset = 0;
    switch (SHAPE(now))
    {
        case 0:
        {
            cookBoxPoints(gdp, now, offset);
        }
        break;
        case 1:
        {
            cookPyramidPoints(gdp, now, offset);
        }
        break;
        case 2:
        {
            cookDiamondPoints(gdp, now, offset);
        }
        break;
    }

    GA_Attribute* normalAttribute = gdp->addFloatTuple(GA_ATTRIB_POINT, "N", 3);
    const GA_AIFTuple* accessTuple = normalAttribute->getAIFTuple();

    fpreal32 normal[] = { 0.0f, 1.0f, 0.0f };

    for (int i = 0; i < offset; ++i)
    {
        UT_Vector3 pos = gdp->getPos3(i);
        if (USEPOS(now) > 0)
        {
            TARGET(normal, now);
            normal[0] -= pos[0];
            normal[1] -= pos[1];
            normal[2] -= pos[2];
        }
        else
        {
            normal[0] = pos[0];
            normal[1] = pos[1];
            normal[2] = pos[2];
        }
        accessTuple->set(normalAttribute, i, normal, 3);
    }

    return UT_ERROR_NONE;
}

OP_ERROR SOP_HDKCookDemo00::cookBoxPoints(GU_Detail* gdp, fpreal t, int& offset)
{
    float scaleX = SCALEX(t);
    float scaleY = SCALEY(t);
    float scaleZ = SCALEZ(t);

    int depth = DEPTH(t);
    int detail = DETAIL(t);

    for (int i = 0; i < depth; ++i)
    {
        UT_Vector3 pos[] =
        {
            UT_Vector3(-scaleX * 0.5f, -scaleY * 0.5f, -scaleZ * 0.5f),
            UT_Vector3(scaleX * 0.5f, -scaleY * 0.5f, -scaleZ * 0.5f),
            UT_Vector3(scaleX * 0.5f, -scaleY * 0.5f, scaleZ * 0.5f),
            UT_Vector3(-scaleX * 0.5f, -scaleY * 0.5f, scaleZ * 0.5f),

            UT_Vector3(-scaleX * 0.5f, scaleY * 0.5f, -scaleZ * 0.5f),
            UT_Vector3(scaleX * 0.5f, scaleY * 0.5f, -scaleZ * 0.5f),
            UT_Vector3(scaleX * 0.5f, scaleY * 0.5f, scaleZ * 0.5f),
            UT_Vector3(-scaleX * 0.5f, scaleY * 0.5f, scaleZ * 0.5f),
        };

        gdp->appendPointBlock(8);

        for (int k = 0; k < 8; ++k)
        {
            gdp->setPos3(offset, pos[k]);
            ++offset;
        }

        if (detail > 1) 
        {
            addDetail(gdp, pos[0], pos[1], offset, detail);
            addDetail(gdp, pos[1], pos[2], offset, detail);
            addDetail(gdp, pos[2], pos[3], offset, detail);
            addDetail(gdp, pos[3], pos[0], offset, detail);
            addDetail(gdp, pos[4], pos[5], offset, detail);
            addDetail(gdp, pos[5], pos[6], offset, detail);
            addDetail(gdp, pos[6], pos[7], offset, detail);
            addDetail(gdp, pos[7], pos[4], offset, detail);
            addDetail(gdp, pos[0], pos[4], offset, detail);
            addDetail(gdp, pos[1], pos[5], offset, detail);
            addDetail(gdp, pos[2], pos[6], offset, detail);
            addDetail(gdp, pos[3], pos[7], offset, detail);

            UT_Vector3 dv01 = (pos[1] - pos[0]) / detail;
            UT_Vector3 dv30 = (pos[0] - pos[3]) / detail;
            UT_Vector3 dv04 = (pos[4] - pos[0]) / detail;

            for (int d = 1; d < detail; ++d)
            {
                addDetail(gdp, pos[0] + dv01 * d, pos[4] + dv01 * d, offset, detail);
                addDetail(gdp, pos[3] + dv01 * d, pos[7] + dv01 * d, offset, detail);
                addDetail(gdp, pos[2] + dv30 * d, pos[6] + dv30 * d, offset, detail);
                addDetail(gdp, pos[3] + dv30 * d, pos[7] + dv30 * d, offset, detail);
                addDetail(gdp, pos[3] + dv01 * d, pos[0] + dv01 * d, offset, detail);
                addDetail(gdp, pos[7] + dv01 * d, pos[4] + dv01 * d, offset, detail);
            }
        }

        scaleX = scaleX * DEPTHSCALE(t);
        scaleY = scaleY * DEPTHSCALE(t);
        scaleZ = scaleZ * DEPTHSCALE(t);
    }

    return UT_ERROR_NONE;
}

OP_ERROR SOP_HDKCookDemo00::cookPyramidPoints(GU_Detail* gdp, fpreal t, int& offset)
{
    float scaleX = SCALEX(t);
    float scaleY = SCALEY(t);
    float scaleZ = SCALEZ(t);

    int depth = DEPTH(t);
    int detail = DETAIL(t);

    for (int i = 0; i < depth; ++i)
    {
        UT_Vector3 pos[] =
        {
            UT_Vector3(-scaleX * 0.5f, -scaleY * 0.5f, -scaleZ * 0.5f),
            UT_Vector3(scaleX * 0.5f, -scaleY * 0.5f, -scaleZ * 0.5f),
            UT_Vector3(scaleX * 0.5f, -scaleY * 0.5f, scaleZ * 0.5f),
            UT_Vector3(-scaleX * 0.5f, -scaleY * 0.5f, scaleZ * 0.5f),
            UT_Vector3(0.0f, scaleY, 0.0f),
        };

        gdp->appendPointBlock(5);

        for (int k = 0; k < 5; ++k)
        {
            gdp->setPos3(offset, pos[k]);
            ++offset;
        }

        if (detail > 1)
        {
            addDetail(gdp, pos[0], pos[1], offset, detail);
            addDetail(gdp, pos[1], pos[2], offset, detail);
            addDetail(gdp, pos[2], pos[3], offset, detail);
            addDetail(gdp, pos[3], pos[0], offset, detail);
            addDetail(gdp, pos[0], pos[4], offset, detail);
            addDetail(gdp, pos[1], pos[4], offset, detail);
            addDetail(gdp, pos[2], pos[4], offset, detail);
            addDetail(gdp, pos[3], pos[4], offset, detail);

            UT_Vector3 dv01 = (pos[1] - pos[0]) / detail;
            UT_Vector3 dv30 = (pos[0] - pos[3]) / detail;

            for (int d = 1; d < detail; ++d)
            {
                addDetail(gdp, pos[0] + dv01 * d, pos[4], offset, detail);
                addDetail(gdp, pos[2] + dv30 * d, pos[4], offset, detail);
                addDetail(gdp, pos[3] + dv01 * d, pos[4], offset, detail);
                addDetail(gdp, pos[3] + dv30 * d, pos[4], offset, detail);
                addDetail(gdp, pos[3] + dv01 * d, pos[0] + dv01 * d, offset, detail);
            }

        }

        scaleX = scaleX * DEPTHSCALE(t);
        scaleY = scaleY * DEPTHSCALE(t);
        scaleZ = scaleZ * DEPTHSCALE(t);
    }

    return UT_ERROR_NONE;
}

OP_ERROR SOP_HDKCookDemo00::cookDiamondPoints(GU_Detail* gdp, fpreal t, int& offset)
{
    float scaleX = SCALEX(t);
    float scaleY = SCALEY(t);
    float scaleZ = SCALEZ(t);

    int depth = DEPTH(t);
    int detail = DETAIL(t);

    for (int i = 0; i < depth; ++i)
    {
        UT_Vector3 pos[] =
        {
            UT_Vector3(-scaleX * 0.5f, 0.0f, -scaleZ * 0.5f),
            UT_Vector3(scaleX * 0.5f, 0.0f, -scaleZ * 0.5f),
            UT_Vector3(scaleX * 0.5f, 0.0f, scaleZ * 0.5f),
            UT_Vector3(-scaleX * 0.5f, 0.0f, scaleZ * 0.5f),
            UT_Vector3(0.0f, scaleY * 0.5, 0.0f),
            UT_Vector3(0.0f, -scaleY * 0.5, 0.0f),
        };

        gdp->appendPointBlock(6);
        for (int k = 0; k < 6; ++k)
        {
            gdp->setPos3(offset, pos[k]);
            ++offset;
        }

        if (detail > 1)
        {
            addDetail(gdp, pos[0], pos[1], offset, detail);
            addDetail(gdp, pos[1], pos[2], offset, detail);
            addDetail(gdp, pos[2], pos[3], offset, detail);
            addDetail(gdp, pos[3], pos[0], offset, detail);
            addDetail(gdp, pos[0], pos[4], offset, detail);
            addDetail(gdp, pos[1], pos[4], offset, detail);
            addDetail(gdp, pos[2], pos[4], offset, detail);
            addDetail(gdp, pos[3], pos[4], offset, detail);
            addDetail(gdp, pos[0], pos[5], offset, detail);
            addDetail(gdp, pos[1], pos[5], offset, detail);
            addDetail(gdp, pos[2], pos[5], offset, detail);
            addDetail(gdp, pos[3], pos[5], offset, detail);

            UT_Vector3 dv01 = (pos[1] - pos[0]) / detail;
            UT_Vector3 dv30 = (pos[0] - pos[3]) / detail;

            for (int d = 1; d < detail; ++d)
            {
                addDetail(gdp, pos[0] + dv01 * d, pos[4], offset, detail);
                addDetail(gdp, pos[2] + dv30 * d, pos[4], offset, detail);
                addDetail(gdp, pos[3] + dv01 * d, pos[4], offset, detail);
                addDetail(gdp, pos[3] + dv30 * d, pos[4], offset, detail);
                addDetail(gdp, pos[0] + dv01 * d, pos[5], offset, detail);
                addDetail(gdp, pos[2] + dv30 * d, pos[5], offset, detail);
                addDetail(gdp, pos[3] + dv01 * d, pos[5], offset, detail);
                addDetail(gdp, pos[3] + dv30 * d, pos[5], offset, detail);
            }
        }

        scaleX = scaleX * DEPTHSCALE(t);
        scaleY = scaleY * DEPTHSCALE(t);
        scaleZ = scaleZ * DEPTHSCALE(t);
    }

    return UT_ERROR_NONE;
}

OP_ERROR SOP_HDKCookDemo00::addDetail(GU_Detail* gdp, UT_Vector3 p0, UT_Vector3 p1, int& offset, int detail)
{
    gdp->appendPointBlock(detail - 1);
    UT_Vector3 v = (p1 - p0) / detail;
    for (int d = 1; d < detail; ++d)
    {
        gdp->setPos3(offset, p0 + v * d);
        ++offset;
    }
    return UT_ERROR_NONE;
}



