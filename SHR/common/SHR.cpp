/**
 \file 		millionaire_prob.cpp
 \author 	sreeram.sadasivam@cased.de
 \copyright	ABY - A Framework for Efficient Mixed-protocol Secure Two-party Computation
			Copyright (C) 2019 Engineering Cryptographic Protocols Group, TU Darmstadt
			This program is free software: you can redistribute it and/or modify
            it under the terms of the GNU Lesser General Public License as published
            by the Free Software Foundation, either version 3 of the License, or
            (at your option) any later version.
            ABY is distributed in the hope that it will be useful,
            but WITHOUT ANY WARRANTY; without even the implied warranty of
            MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
            GNU Lesser General Public License for more details.
            You should have received a copy of the GNU Lesser General Public License
            along with this program. If not, see <http://www.gnu.org/licenses/>.
 \brief		Implementation of the millionaire problem using ABY Framework.
 */

#include <iostream>

#include "SHR.h"
#include "../../../abycore/circuit/booleancircuits.h"
#include "../../../abycore/sharing/sharing.h"

using namespace std;

class SHR{
    // SHR class define
    public:
        // constructor
        SHR(uint64_t* data,BooleanCircuit* bct,ABYParty* par,e_role role);
        // get clear value i
        double check();
    private:
        ABYParty* party;
        BooleanCircuit* bc;
        uint32_t bitlen;
        uint64_t i;
        double d;
        bool cons;  // 1: const; 0: other
        SHR* pself; 
};

//plaintext
SHR::SHR(uint64_t* data,BooleanCircuit* bct,ABYParty* par,e_role role) {
    bitlen = 64;
    cons = 0;
    party = par;
    bc = bct;
    //share data
    share *tmp,*out;
    // input
    if(data == NULL){
        tmp = bc->PutDummyINGate(bitlen);
    }else{
        tmp = bc->PutINGate(data[0],bitlen,role);
    }
    // output
    out = bc->PutSharedOUTGate(tmp);

    party->ExecCircuit();
    // get clear value
    i = out->get_clear_value<uint64_t>();
    d= *(double*)& i;
    // reset ABYParty
    party->Reset();
}
double SHR::check(){
    if(cons){
        // if it is const, get d, not i
        // return this->get_d();
        return 0;
    }else{
        ABYParty* party = this->party;
        BooleanCircuit* bc = this->bc;
        uint64_t result;
        share *inp,*oup;
        // take i
        inp = bc->PutSharedINGate(this->i , this->bitlen);
        // output i to both side
        oup = bc->PutOUTGate(inp,ALL);
        party->ExecCircuit();
        // get clear value
        result = oup->get_clear_value<uint64_t>();
        party->Reset();
        // return double i
        return *(double*)&result;
    }
}

int32_t test_SHR_circuit(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
		uint32_t bitlen, uint32_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing) {
    // set basic info
    ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg, 100000, "/home/howcat/ABY/bin/circ/");
	std::vector<Sharing*>& sharings = party->GetSharings();
	Circuit* circ = sharings[sharing]->GetCircuitBuildRoutine();

    // input for test
    uint64_t test_data = 1010;

    // declare SHR class
    SHR test(&test_data, (BooleanCircuit*) circ, party, role);
    // test SHR.check()
    std::cout << test.check() << '\n';

    /*
    We get some trouble with ExecCircuit() in SHR class constructor and check().
    It will stop running, and we cannot comment that part.
    We will take a rest today.
    */

    return 0;
}
