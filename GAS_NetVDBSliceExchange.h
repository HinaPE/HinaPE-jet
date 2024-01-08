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

#ifndef __GAS_NetVDBSliceExchange__
#define __GAS_NetVDBSliceExchange__

#include <GAS/GAS_SubSolver.h>
#include <GAS/GAS_Utils.h>

namespace HDK_Sample {

class GAS_NetVDBSliceExchange : public GAS_SubSolver
{
public:
    GET_DATA_FUNC_S("vdbname", VDBName);
    
    GET_DATA_FUNC_S(GAS_NAME_TRACKERADDR, TrackerAddress);
    GET_DATA_FUNC_I(GAS_NAME_TRACKERPORT, TrackerPort);
    GET_DATA_FUNC_S(GAS_NAME_JOBNAME, JobName);

    GET_DATA_FUNC_I("slice", Slice);
    GET_DATA_FUNC_S("sliceroot", SliceRoot);

    GET_DATA_FUNC_F("overlap", Overlap);
    GET_DATA_FUNC_F("overlapvoxel", OverlapVoxel);

protected:
    explicit		 GAS_NetVDBSliceExchange(const SIM_DataFactory *factory);
                        ~GAS_NetVDBSliceExchange() override;

    /// Moves the fields given by VDBName by the given velocity
    /// field.  Moves the points given by GeometryName likewise.
    bool                 solveGasSubclass(SIM_Engine &engine,
				SIM_Object *obj,
				SIM_Time time,
				SIM_Time timestep) override;

private:
    static const SIM_DopDescription	*getDopDescription();

    DECLARE_STANDARD_GETCASTTOTYPE();
    DECLARE_DATAFACTORY(GAS_NetVDBSliceExchange,
			GAS_SubSolver,
			"Gas Net VDB Slice Exchange",
			getDopDescription());
};

} // End HDK_Sample namespace

#endif

