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
#include <string>
#include <vector>
#include <sstream>

#include "project_2.h"
#include "../../../abycore/circuit/booleancircuits.h"
#include "../../../abycore/sharing/sharing.h"

using namespace std;

int32_t test_project_2_circuit(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
		uint32_t bitlen, uint32_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing) {

	/**
		Step 1: Create the ABYParty object which defines the basis of all the
		 	 	operations which are happening.	Operations performed are on the
		 	 	basis of the role played by this object.
	*/
	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg, 100000, "/home/howcat/ABY/bin/circ/");


	/**
		Step 2: Get to know all the sharing types available in the program.
	*/

	std::vector<Sharing*>& sharings = party->GetSharings();

	/**
		Step 3: Create the circuit object on the basis of the sharing type
				being inputed.
	*/
	Circuit* circ = sharings[sharing]->GetCircuitBuildRoutine();


	/**
		Step 4: Creating the share objects - s_alice_money, s_bob_money which
				is used as input to the computation function. Also s_out
				which stores the output.
	*/

	share *s_alice_money, *s_bob_money, *s_out;

	/**
		Step 5: Initialize Alice's and Bob's money with random values.
				Both parties use the same seed, to be able to verify the
				result. In a real example each party would only supply
				one input value.
	*/

	uint32_t alice_money, bob_money, output;
    string bob_money_temp;
	srand(time(NULL));
    // two money
	// alice_money = rand();
	// bob_money = rand();
    alice_money = 11;
    bob_money_temp = "0.00000000000000";

    vector<string> spilt_result;
    stringstream ss(bob_money_temp);
    string tok;

    while(getline(ss, tok, '.')){
        spilt_result.push_back(tok);
    }
    bob_money = stoi(spilt_result[1]);

	/**
		Step 6: Copy the randomly generated money into the respective
				share objects using the circuit object method PutINGate()
				for my inputs and PutDummyINGate() for the other parties input.
				Also mention who is sharing the object.
	*/
	//s_alice_money = circ->PutINGate(alice_money, bitlen, CLIENT);
	//s_bob_money = circ->PutINGate(bob_money, bitlen, SERVER);
	if(role == SERVER) {
		s_alice_money = circ->PutDummyINGate(bitlen);
		s_bob_money = circ->PutINGate(bob_money, bitlen, SERVER);
	} else { //role == CLIENT
		s_alice_money = circ->PutINGate(alice_money, bitlen, CLIENT);
		s_bob_money = circ->PutDummyINGate(bitlen);
	}

	/**
		Step 7: Call the build method for building the circuit for the
				problem by passing the shared objects and circuit object.
				Don't forget to type cast the circuit object to type of share
	*/

	s_out = BuildCeilCircuit(party, s_alice_money, s_bob_money,
			(BooleanCircuit*) circ);

	/**
		Step 8: Modify the output receiver based on the role played by
				the server and the client. This step writes the output to the
				shared output object based on the role.
	*/
	s_out = circ->PutOUTGate(s_out, ALL);

	/**
		Step 9: Executing the circuit using the ABYParty object evaluate the
				problem.
	*/
	party->ExecCircuit();

	/**
		Step 10:Type casting the value to 32 bit unsigned integer for output.
	*/
	output = s_out->get_clear_value<uint32_t>();

	std::cout << "Testing Ceil() in " << get_sharing_name(sharing)
				<< " sharing: " << std::endl;
    std::cout << "Ceil function result is:\t" <  output << std::endl;

	delete party;
	return 0;
}

share* BuildCeilCircuit(ABYParty* party, share *s_alice, share *s_bob,
		BooleanCircuit *bc) {
    uint32_t one = 1, zero = 0, judge_result = 0;
	share *s_one = bc->PutCONSGate(one,32), *s_zero = bc->PutCONSGate(zero,32);
    share *tmp1,*out,*tmp2,*tmp3;

    tmp1 = bc->PutGTGate(s_bob,s_zero);
    tmp2 = bc->PutGTGate(s_zero,s_bob);
    tmp3 = bc->PutXORGate(tmp1,tmp2);
    s_alice = bc->PutADDGate(s_alice, tmp3);

	return s_alice;
}
