/*
 * Copyright (c) 2023
 *	Side Effects Software Inc.  All rights reserved.
 *
 * Redistribution and use of Houdini Development Kit samples in source and
 * binary forms, with or without modification, are permitted provided that the
 * following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. The name of Side Effects Software may not be used to endorse or
 *    promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY SIDE EFFECTS SOFTWARE `AS IS' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 * NO EVENT SHALL SIDE EFFECTS SOFTWARE BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *----------------------------------------------------------------------------
 */

#include "GAS_NetVDBSliceExchange.h"

#include <UT/UT_DSOVersion.h>
#include <UT/UT_Interrupt.h>
#include <UT/UT_NetMessage.h>
#include <UT/UT_StringStream.h>

#include <SIM/SIM_PRMShared.h>
#include <SIM/SIM_DopDescription.h>
#include <SIM/SIM_Object.h>
#include <SIM/SIM_GeometryCopy.h>
#include <SIM/SIM_Slice.h>

#include <GEO/GEO_PrimVolume.h>
#include <GU/GU_PrimVDB.h>

using namespace HDK_Sample;

///
/// This is the hook that Houdini grabs from the dll to link in
/// this.  As such, it merely has to implement the data factory
/// for this node.
///
//void
//initializeSIM(void *)
//{
//    IMPLEMENT_DATAFACTORY(GAS_NetVDBSliceExchange);
//}

//
// GAS_NetVDBSliceExchange
//
GAS_NetVDBSliceExchange::GAS_NetVDBSliceExchange(const SIM_DataFactory *factory)
    : BaseClass(factory)
{
}

GAS_NetVDBSliceExchange::~GAS_NetVDBSliceExchange()
{
}

const SIM_DopDescription *
GAS_NetVDBSliceExchange::getDopDescription()
{
    static PRM_Name	theGeometryName(GAS_NAME_GEOMETRY, "Geometry");
    static PRM_Default	theGeometryNameDefault(0, "Geometry"); 
    static PRM_Name	theVDBName("vdbname", "VDB Names");
    static PRM_Default  theVDBNameDefault(0, "*");

    static PRM_Name 	theAddrName(GAS_NAME_TRACKERADDR, "Tracker Address");
    static PRM_Name 	thePortName(GAS_NAME_TRACKERPORT, "Tracker Port");
    static PRM_Name	theJobName(GAS_NAME_JOBNAME, "Job Name");
    static PRM_Default	thePortDefault(8000);
    static PRM_Default	theJobDefault(0, "${OS}_$F");
    static PRM_Name	theThisSliceName("slice", "Slice Number");

    static PRM_Name	theSliceRootName("sliceroot", "Slice Data Root");
    static PRM_Default	theSliceRootDefault(0, "Slice");

    static PRM_Name	theOverlapName("overlap", "Slice Overlap");
    static PRM_Name	theOverlapVoxelName("overlapvoxel", "Voxel Overlap");

    static PRM_Template theTemplates[] = {
        PRM_Template(PRM_STRING, 1, &theGeometryName, &theGeometryNameDefault),
	PRM_Template(PRM_STRING, 1, &theVDBName, &theVDBNameDefault),

	PRM_Template(PRM_STRING,	1, &theAddrName),
	PRM_Template(PRM_INT,		1, &thePortName, &thePortDefault),
	PRM_Template(PRM_STRING,	1, &theJobName, &theJobDefault),
	PRM_Template(PRM_INT,	1, &theThisSliceName, PRMzeroDefaults),

	PRM_Template(PRM_STRING,	1, &theSliceRootName, &theSliceRootDefault),

	PRM_Template(PRM_FLT,		1, &theOverlapName, PRMzeroDefaults,
		    0, 0, 0, &PRM_SpareData::unitsLength ),
	PRM_Template(PRM_FLT,		1, &theOverlapVoxelName, PRMzeroDefaults),
	PRM_Template()
    };

    static SIM_DopDescription	theDopDescription(true,
					"hdk_gasnetvdbsliceexchange",
					"Gas Net VDB Slice Exchange",
					"$OS",
					classname(),
					theTemplates);
    setGasDescription(theDopDescription);

    return &theDopDescription;
}

static void
GASreadDataFromPacket(fpreal32 &value, UT_NetMessage *msg, exint offset)
{
    value = msg->extractFloat32(offset);
}

static void
GASreadDataFromPacket(fpreal64 &value, UT_NetMessage *msg, exint offset)
{
    value = msg->extractFloat64(offset);
}

static void
GASreadDataFromPacket(int32 &value, UT_NetMessage *msg, exint offset)
{
    value = msg->extractInt32(offset);
}

static void
GASreadDataFromPacket(int64 &value, UT_NetMessage *msg, exint offset)
{
    value = msg->extractInt64(offset);
}

static void
GASreadDataFromPacket(openvdb::math::Vec3<float> &value, UT_NetMessage *msg, exint offset)
{
    value[0] = msg->extractFloat32(offset);
    value[1] = msg->extractFloat32(offset + sizeof(value[0]));
    value[2] = msg->extractFloat32(offset + 2*sizeof(value[0]));
}

static void
GASreadDataFromPacket(openvdb::math::Vec3<double> &value, UT_NetMessage *msg, exint offset)
{
    value[0] = msg->extractFloat64(offset);
    value[1] = msg->extractFloat64(offset + sizeof(value[0]));
    value[2] = msg->extractFloat64(offset + 2*sizeof(value[0]));
}

static void
GASreadDataFromPacket(openvdb::math::Vec3<int32> &value, UT_NetMessage *msg, exint offset)
{
    value[0] = msg->extractInt32(offset);
    value[1] = msg->extractInt32(offset + sizeof(value[0]));
    value[2] = msg->extractInt32(offset + 2*sizeof(value[0]));
}

/*
static void
GASreadDataFromPacket(openvdb::math::Vec3<int64> &value, UT_NetMessage *msg, exint offset)
{
    value[0] = msg->extractInt64(offset);
    value[1] = msg->extractInt64(offset + sizeof(value[0]));
    value[2] = msg->extractInt64(offset + 2*sizeof(value[0]));
}
*/

static void
GASwriteDataToPacket(UT_NetMessage *msg, exint offset, fpreal32 value)
{
    msg->overwriteFloat32(offset, value);
}

static void
GASwriteDataToPacket(UT_NetMessage *msg, exint offset, fpreal64 value)
{
    msg->overwriteFloat64(offset, value);
}

static void
GASwriteDataToPacket(UT_NetMessage *msg, exint offset, int32 value)
{
    msg->overwriteInt32(offset, value);
}

static void
GASwriteDataToPacket(UT_NetMessage *msg, exint offset, int64 value)
{
    msg->overwriteInt64(offset, value);
}

static void
GASwriteDataToPacket(UT_NetMessage *msg, exint offset, openvdb::math::Vec3<float> value)
{
    msg->overwriteFloat32(offset, value[0]);
    msg->overwriteFloat32(offset+sizeof(value[0]), value[1]);
    msg->overwriteFloat32(offset+2*sizeof(value[0]), value[2]);
}

static void
GASwriteDataToPacket(UT_NetMessage *msg, exint offset, openvdb::math::Vec3<double> value)
{
    msg->overwriteFloat64(offset, value[0]);
    msg->overwriteFloat64(offset+sizeof(value[0]), value[1]);
    msg->overwriteFloat64(offset+2*sizeof(value[0]), value[2]);
}

static void
GASwriteDataToPacket(UT_NetMessage *msg, exint offset, openvdb::math::Vec3<int32> value)
{
    msg->overwriteInt32(offset, value[0]);
    msg->overwriteInt32(offset+sizeof(value[0]), value[1]);
    msg->overwriteInt32(offset+2*sizeof(value[0]), value[2]);
}

/*
static void
GASwriteDataToPacket(UT_NetMessage *msg, exint offset, openvdb::math::Vec3<int64> value)
{
    msg->overwriteInt64(offset, value[0]);
    msg->overwriteInt64(offset+sizeof(value[0]), value[1]);
    msg->overwriteInt64(offset+2*sizeof(value[0]), value[2]);
}
*/

template <typename GridType>
void
GASnetvdbsliceexchangeApplyPacketToVDB(
        GridType &grid,
	GAS_NetVDBSliceExchange *solver, SIM_Object *obj,
	UT_NetMessage *msg)
{
    int srcpeer = msg->extractInt16(6);

    typename GridType::Accessor acc = grid.getAccessor();
    using ValueType = typename GridType::ValueType;

    exint		numreply;
    numreply = msg->extractInt32(8);

    // Where we are reading packets.
    exint		replyoff = 16;

    for (exint reply = 0; reply < numreply; reply++)
    {
	if (replyoff >= msg->length())
	{
	    std::cerr <<  "Malformed reply from " << srcpeer << ", total replies " << numreply << " exhausted message packet sized " << msg->length() << std::endl;
	    return;
	}

	UT_Vector3I		minvxl;
	minvxl.x() = msg->extractInt32(replyoff+0);
	minvxl.y() = msg->extractInt32(replyoff+4);
	minvxl.z() = msg->extractInt32(replyoff+8);
        int magictoken = msg->extractInt32(replyoff+12);
        if (magictoken != 0x51DEEFC5)
        {
            // Corrupted data stream or we lost count, we should
            // abandon
	    std::cerr <<  "Malformed reply from " << srcpeer << ", total replies " << numreply << " missing magic token on packet " << reply << " at offset " << replyoff << std::endl;
            return;
        }

        char nodemask[64];
        for (int nm = 0; nm < 64; nm++)
            nodemask[nm] = msg->extractInt8(replyoff+16+nm);

        // Get to data.
        replyoff += 16 + 64;
        int nodemaskpos = 0;
        exint nodeoff = 0;
        for (int x = 0; x < 8; x++)
        {
            for (int y = 0; y < 8; y++)
            {
                int flag = 1;
                for (int z = 0; z < 8; z++)
                {
                    if (nodemask[nodemaskpos] & flag)
                    {
                        // Read in a value and store in grid
                        ValueType v;

                        GASreadDataFromPacket(v, msg, replyoff+nodeoff);
                        openvdb::Coord coord(minvxl.x() + x, minvxl.y() + y, minvxl.z() + z);
                        acc.setValueOn(coord, v);
                        nodeoff += sizeof(v);
                    }
                    flag += flag;
                }
                nodemaskpos++;
            }
        }

        replyoff += nodeoff;
    }
}

void
GASnetvdbsliceexchangeApplyPacket(
        GU_PrimVDB *vdb,
	GAS_NetVDBSliceExchange *solver, SIM_Object *obj,
	UT_NetMessage *msg)
{
    UTvdbCallAllType(vdb->getStorageType(),
            GASnetvdbsliceexchangeApplyPacketToVDB,
            vdb->getGrid(),
            solver, obj, msg);
}

template <typename GridType>
void
GASnetvdbsliceexchangeBuildPacketGrid(GridType &grid,
        const GEO_PrimVolumeXform &indexxform,
        UT_Array<UT_NetMessage *> &msgs, 
	GAS_NetVDBSliceExchange *solver, SIM_Object *obj,
	fpreal overlap,
	exint thisslice, const SIM_SliceSearchTable &slicetable)
{
    typename GridType::Accessor acc = grid.getAccessor();
    using ValueType = typename GridType::ValueType;

    UT_ExintArray        msgpackets, msgoffsets;
    int                  numslice = msgs.entries();
    for (int i = 0; i < numslice; i++)
    {
        msgpackets.append(0);
        msgoffsets.append(16);
        if (i != thisslice)
        {
            msgs(i) = new UT_NetMessage();
            // first 8 is standard header
            // 4 for numreplies
            // 4 for reserved.
            msgs(i)->setWriteDataLength(8 + 8);
        }
    }

    UT_Array<UT_IntArray>       packetvoxeloffset;
    UT_Array<UT_Array<ValueType>> packetvoxelvalue;
    packetvoxeloffset.setSize(numslice);
    packetvoxelvalue.setSize(numslice);

    ValueType background = grid.background();

    for (auto it = grid.tree().beginLeaf(); it; ++it)
    {
        // Get voxel bounds.
        auto vdb_box = it->getNodeBoundingBox();

	UT_Vector3		tstart, tend;
	UT_BoundingBox		tbbox, tbbox_expand;

        tstart.assign(vdb_box.min().x()-0.5, vdb_box.min().y()-0.5, vdb_box.min().z()-0.5);
        tend.assign(vdb_box.max().x()+0.5, vdb_box.max().y()+0.5, vdb_box.max().z()+0.5);
        tbbox.initBounds(tstart);
        tbbox.enlargeBounds(tend);
        indexxform.fromVoxelSpace(tbbox);

	tbbox_expand = tbbox;
	tbbox_expand.expandBounds(overlap, overlap, overlap);

        // If our expanded box is entirely inside our slice we have no
        // voxels to deactivate or transmit (As external slices are only
        // interested in a distance up to overlap)
        if (SIM_Slice::isInsideSlice(slicetable(thisslice), tbbox_expand))
        {
            continue;
        }

        // If our tight bounding box is entirely outside of our slice
        // we have no voxels to transmit and all our voxels must be
        // de-activated.
        if (!SIM_Slice::doesIntersectSlice(slicetable(thisslice), tbbox))
        {
            for (int i = vdb_box.min().x(); i <= vdb_box.max().x(); i++)
                for (int j = vdb_box.min().y(); j <= vdb_box.max().y(); j++)
                    for (int k = vdb_box.min().z(); k <= vdb_box.max().z(); k++)
                        acc.setValueOff(openvdb::Coord(i, j, k), background);
            continue;
        }

        // We have at box that has some voxels that may need to
        // be transferred.
        // Given a slice topology there often is only a few potential
        // candidates, but so long as total machines is small there
        // isn't much gain in overthinking this.

        for (int slice = 0; slice < numslice; slice++)
        {
            packetvoxeloffset(slice).entries(0);
            packetvoxelvalue(slice).entries(0);
        }

        int     offset = 0;
        for (int i = vdb_box.min().x(); i <= vdb_box.max().x(); i++)
            for (int j = vdb_box.min().y(); j <= vdb_box.max().y(); j++)
                for (int k = vdb_box.min().z(); k <= vdb_box.max().z(); k++)
                {
                    UT_Vector3          pos(i, j, k);
                    pos = indexxform.fromVoxelSpace(pos);

                    if (!SIM_Slice::isInsideSlice(slicetable(thisslice), pos))
                    {
                        // This coordinate is outside our slice, so we
                        // do not own it.  We thus de-activate it.
                        acc.setValueOff(openvdb::Coord(i, j, k), background);
                    }
                    else
                    {
                        // Test each slice to see if we need to transfer
                        // to that slice.
                        for (int slice = 0; slice < numslice; slice++)
                        {
                            // Don't send to ourself.
                            if (slice == thisslice)
                                continue;

                            // See if the given slice is within overlap
                            if (SIM_Slice::computeSliceDist(slicetable(slice), pos) < overlap)
                            {
                                packetvoxeloffset(slice).append( offset );
                                packetvoxelvalue(slice).append( acc.getValue(openvdb::Coord(i, j, k)) );
                            }
                        }
                    }

                    offset++;
                }

        // We've now bulid the packetvoxeloffset & value lists.
        // For any that are non zero we want to add a packet.
        for (int slice = 0; slice < numslice; slice++)
        {
            if (slice == thisslice)
                continue;

            if (packetvoxeloffset(slice).entries() == 0)
                continue;

            auto && msg = msgs(slice);
            
            exint msgoffset = msgoffsets(slice);
            msgpackets(slice)++;

            msg->growWriteData(msgoffset + 16 + 64 + sizeof(ValueType) * packetvoxeloffset(slice).entries());

            msg->overwriteInt32(msgoffset + 0, vdb_box.min().x());
            msg->overwriteInt32(msgoffset + 4, vdb_box.min().y());
            msg->overwriteInt32(msgoffset + 8, vdb_box.min().z());
            msg->overwriteInt32(msgoffset + 12, 0x51DEEFC5);

            msgoffset += 16;

            char nodemask[64];
            memset(nodemask, 0, 64);
            for (auto && offset : packetvoxeloffset(slice))
            {
                nodemask[offset >> 3] |= 1 << (offset & 7);
            }

            for (int nm = 0; nm < 64; nm++)
                msg->overwriteInt8(msgoffset + nm, nodemask[nm]);

            msgoffset += 64;

            for (auto && value : packetvoxelvalue(slice))
            {
                GASwriteDataToPacket(msg, msgoffset, value);
                msgoffset += sizeof(value);
            }

            // Write back the new offset.
            msgoffsets(slice) = msgoffset;
        }
    }

    // Write out the number of backets in each message.
    for (int slice = 0; slice < numslice; slice++)
    {
        if (slice == thisslice)
            continue;

        msgs(slice)->overwriteInt32(8, msgpackets(slice));
    }
}

void
GASnetvdbsliceexchangeBuildPacket(GU_PrimVDB *vdb, 
        UT_Array<UT_NetMessage *> &msgs, 
	GAS_NetVDBSliceExchange *solver, SIM_Object *obj,
	fpreal overlap,
	exint thisslice, const SIM_SliceSearchTable &slicetable)
{
    UTvdbCallAllType(vdb->getStorageType(),
            GASnetvdbsliceexchangeBuildPacketGrid,
            vdb->getGrid(),
            vdb->getIndexSpaceTransform(),
            msgs,
            solver, obj,
            overlap, thisslice, slicetable);
}

void
GASnetvdbsliceexchangeNetExchange(GAS_NetVDBSliceExchange *solver, 
		    SIM_Object *obj,
		    GU_PrimVDB *vdb,
		    const char *tracker, int port, const char *jobname,
		    exint thisslice,
		    const SIM_SliceSearchTable &slicetable)
{
    exint nslice = slicetable.entries();

    // Trivial to sync unsliced fields.
    if (nslice <= 1)
	return;

    vdb->makeGridUnique();

    fpreal			overlap = solver->getOverlap();

    overlap += solver->getOverlapVoxel() * vdb->getVoxelDiameter();

    //
    // dispatch
    // 	32, 32, 32 - world voxel bottom left
    //  32 - magic number
    //  64*8 - voxel mask
    //  sizeof*active - values
    //

    UT_NetExchange	netxchg(tracker, port, thisslice, nslice, jobname);
    UT_Array<UT_NetMessage *>   dispatchmsgs;
    UT_ValArray<UT_NetMessage *>completed;

    dispatchmsgs.setSize(nslice);

    // Build all of our messages and deactivate outside voxels:
    GASnetvdbsliceexchangeBuildPacket(vdb, dispatchmsgs,
                    solver, obj,
                    overlap,
                    thisslice, slicetable);
    
    // Send requests to all of our peers.
    for (int slice = 0; slice < nslice; slice++)
    {
	if (slice == thisslice)
	    continue;

	netxchg.sendData(slice, dispatchmsgs(slice));
    }

    // We expect a message, possibly empty, from all peers except
    // ourself.
    netxchg.receiveDataLoop(completed, nslice -1);

    // Apply all of our done packets.
    for (int i = 0; i < completed.entries(); i++)
    {
	UT_NetMessage *msg = completed(i);
	completed(i) = 0;
	GASnetvdbsliceexchangeApplyPacket(vdb, solver, obj, msg);
	delete msg;
    }
}

bool
GAS_NetVDBSliceExchange::solveGasSubclass(SIM_Engine &engine,
			SIM_Object *obj,
			SIM_Time time,
			SIM_Time timestep)
{
    SIM_GeometryCopy            *geometry = 0;

    int				 port;
    UT_String			 address, jobname;
    UT_WorkBuffer		 exchangename;

    port = getTrackerPort();
    getTrackerAddress(address);
    getJobName(jobname);

    // No valid host means we should not do any exchange.
    if (!address.isstring())
	return true;

    UT_String			 sliceroot;
    SIM_ConstDataArray		 slices;
    SIM_SliceSearchTable	 slicetable;

    exint slice = getSlice();
    getSliceRoot(sliceroot);

    // Get our slice list.
    obj->filterConstSubData(slices, 0, SIM_DataFilterByType("SIM_Slice"),
			    sliceroot, SIM_DataFilterNone());

    SIM_Slice::buildSliceSearch(slicetable, slices);

    exint numslice = slicetable.entries();
    if (numslice <= 1)
    {
        // Trivial, no slices.
        return true;
    }

    geometry = getGeometryCopy(obj, GAS_NAME_GEOMETRY);

    // Nothing to do if no geometry.
    if (!geometry)
        return true;

    SIM_GeometryAutoWriteLock       lock(geometry);
    GU_Detail &gdp = lock.getGdp();

    UT_Array<GEO_Primitive *>       namedprims;
    UT_String vdbname;
    getVDBName(vdbname);

    // Stage 1:
    // Determine what primitives we want to sync, their names
    // and types.

    gdp.findAllPrimitivesByName(namedprims, vdbname);

    UT_Array<GU_PrimVDB *>          vdblist;
    UT_StringArray                  namelist;
    UT_Array<UT_VDBType>            typelist;
    GA_ROHandleS                    name_h(&gdp, GA_ATTRIB_PRIMITIVE, "name");
    exint                           msglen = 8 + 4;

    for (auto && prim : namedprims)
    {
        if (prim->getTypeId() == GEO_PRIMVDB)
        {
            GU_PrimVDB *vdb = (GU_PrimVDB *) prim;
            vdblist.append(vdb);
            if (!name_h.isValid())
                namelist.append("unnamed");
            else
                namelist.append(name_h.get(prim->getMapOffset()));
            typelist.append( vdb->getStorageType() );
            msglen += 4 + namelist.last().length() + 1;
        }
    }

    // Early exit if nothing to synchronize.
    if (!vdblist.entries())
        return true;

    // Stage 2:
    // Send to slice 0 a list of all primitives and types we want
    // to process.
    UT_WorkBuffer               errormsg;

    // Build an exchange and send to first peer our list.
    {
        exchangename.sprintf("%s_verifyvdblist", (const char *) jobname);
        UT_NetExchange netxchg(address, port, slice, numslice, exchangename.buffer());

        if (slice != 0)
        {
            UT_NetMessage           *msg = 0;

            msg = new UT_NetMessage();

            msg->setWriteDataLength(msglen);
            msg->overwriteInt32(8, vdblist.entries());

            exint msgoff = 8 + 4;

            for (int i =  0; i < vdblist.entries(); i++)
            {
                // Write null terminated name.
                const char *name = namelist(i);
                while (*name)
                {
                    msg->overwriteInt8(msgoff++, *name);
                    name++;
                }
                msg->overwriteInt8(msgoff++, 0);

                // Write out the type.
                msg->overwriteInt32(msgoff, (int32) typelist(i));
                msgoff += sizeof(int32);
            }

            // Send the message 
            netxchg.sendData(/*dstpeer=*/ 0, msg);
        }

        UT_ValArray<UT_NetMessage *>    completed;

        // Slice 0 gets all the other slice messages.
        netxchg.receiveDataLoop(completed, (!slice ? (numslice-1) : 0) );

        if (slice == 0)
        {
            // Build our error message.
            UT_WorkBuffer               name;

            for (int i = 0; i < completed.entries(); i++)
            {
                UT_NetMessage           *msg = 0;

                msg = completed(i);
                completed(i) = 0;

                int srcslice = msg->extractInt16(6);

                int numvdb = msg->extractInt32(8);
                if (numvdb != vdblist.entries())
                {
                    errormsg.appendSprintf("Mismatch VDB count: Slice 0 has %d; but Slice %d has %d.\n", (int) vdblist.entries(), srcslice, numvdb);
                    delete msg;
                    continue;
                }

                exint msgoff = 8 + 4;
                int curvdb = 0;
                bool    ok = true;
                while (ok && curvdb < numvdb && msgoff < msg->length())
                {
                    // Read the name...
                    name.clear();
                    while (msgoff < msg->length())
                    {
                        char c = msg->extractInt8(msgoff++);
                        name.append(c);
                        if (!c)
                            break;
                    }
                    UT_VDBType type = (UT_VDBType) msg->extractInt32(msgoff);
                    msgoff += sizeof(int32);

                    if (name.strcmp(namelist(curvdb)))
                    {
                        // Name mismatch failure.
                        ok = false;
                        errormsg.appendSprintf("Mismatch VDB name. Vdb #%d has name %s in Slice 0 but %s in Slice %d.\n",
                                curvdb, (const char *) namelist(curvdb),
                                name.buffer(), srcslice);
                    }
                    if (type != typelist(curvdb))
                    {
                        ok = false;
                        errormsg.appendSprintf("Mismatch VDB type.  VDB #%d has type %d in Slice 0 but %d in Slice %d.\n",
                                curvdb, (int) typelist(curvdb),
                                (int) type, srcslice);
                    }
                    curvdb++;
                }

                if (ok && (curvdb != numvdb))
                {
                    // Malformed packet.
                    errormsg.appendSprintf("Malformed packet from slice %d; got %d vdbs but expected %d.\n", srcslice, curvdb, numvdb);
                }

                delete msg;
            }
        }
        else
        {
            UT_ASSERT(!completed.entries());
        }
    }

    // Now if we are slice 0 we have an error message that we want
    // to broad cast back out so everyone else can go into an error
    // state rather than just having the system hang.
    //
    // We also output the errormsg to std out, which isn't normally
    // a kosher thing to do but there is a risk traditional errors
    // get eaten on the way to the output driver and distributed sims
    // are almost always run non-interactively.
    if (errormsg.isstring())
    {
        std::cerr << "Error: " << errormsg.buffer() << std::endl;
    }

    {
        exchangename.sprintf("%s_reportvdblist", (const char *) jobname);
        UT_NetExchange netxchg(address, port, slice, numslice, exchangename.buffer());

        if (slice == 0)
        {
            for (int dstslice = 1; dstslice < numslice; dstslice++)
            {
                UT_NetMessage           *msg = 0;

                msg = new UT_NetMessage();

                msg->setWriteDataLength(8 + errormsg.length()+1);

                exint msgoff = 8;
                const char *text = errormsg.buffer();
                while (*text)
                {
                    msg->overwriteInt8(msgoff++, *text);
                    text++;
                }
                msg->overwriteInt8(msgoff++, 0);

                // Send the message 
                netxchg.sendData(dstslice, msg);
            }
        }

        UT_ValArray<UT_NetMessage *>    completed;

        // Expect one message in for every slice except 0.
        netxchg.receiveDataLoop(completed, (slice ? 1 : 0) );

        UT_ASSERT(completed.entries() == (slice != 0));

        for (int i = 0; i < completed.entries(); i++)
        {
            UT_NetMessage           *msg = 0;
            msg = completed(i);
            completed(i) = 0;

            exint msgoff = 8;
            while (msgoff < msg->length())
            {
                char c = msg->extractInt8(msgoff++);
                errormsg.append(c);
                if (!c)
                    break;
            }

            delete msg;
        }

        // Now errormsg is either unchanged (if we are slice 0)
        // or comes from slice 0.  In any case, we can use it
        // to report our error.

        if (UTisstring(errormsg.buffer()))
        {
            addError(obj, SIM_MESSAGE, errormsg.buffer(), UT_ERROR_ABORT);

            return false;
        }

    }

    // TODO: We should first synchronize to verify we have matching
    // names and types before continuing.

    for (auto && vdb : vdblist)
    {
        exchangename.sprintf("%s_%d", (const char *) jobname, (int)vdb->getMapIndex());
        GASnetvdbsliceexchangeNetExchange(this, obj,
                    vdb,
                    address, port, exchangename.buffer(),
                    slice, slicetable);
    }

    return true;
}

