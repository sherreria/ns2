/* -*-	Mode:C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t -*- */
/*
 * Author: Sergio Herreria, Sergio.Herreria@det.uvigo.es 
 * Date: Jun 2014
 * Copyright (C) Sergio Herreria 2014. All rights reserved.
 */

#include "random.h"
#include "trafgen.h"

class Etsi_Traffic : public TrafficGenerator {
public:
	Etsi_Traffic();
	virtual double next_interval(int&);

protected:
	void init();

	double inter_session_time_;      // Average time between two consecutive sessions
	unsigned int calls_per_session_; // Average number of packet call requests per session
	unsigned int packets_per_call_;  // Average number of packets in a packet call
	double reading_time_;            // Average time between two consecutive packet call requests in a session
	double inter_arrival_time_;      // Average time between two consecutive packets inside a packet call
	
	int pareto_size_;                // Is used a Pareto distribution with cut-off to model packet size?
	                                 // True if packetSize_ is set to 0. Pareto parameters: shape=1.1, scale=81.5, maximal=66666
	                                 // Otherwise, packet size is fixed
	unsigned int rem_calls_;         // Remaining number of packet calls in current session
	unsigned int rem_packets_;       // Remaining number of packets in current packet call
};


static class EtsiTrafficClass : public TclClass {
public:
	EtsiTrafficClass() : TclClass("Application/Traffic/Etsi") {}
 	TclObject* create(int, const char*const*) {
		return (new Etsi_Traffic());
	}
} class_etsi_traffic;

Etsi_Traffic::Etsi_Traffic()
{
	bind_time("inter_session_time_", &inter_session_time_);
	bind("calls_per_session_", &calls_per_session_);
	bind_time("reading_time_", &reading_time_);
	bind("packets_per_call_", &packets_per_call_);
	bind_time("inter_arrival_time_", &inter_arrival_time_);
	bind("packetSize_", &size_);
}

void Etsi_Traffic::init()
{
	pareto_size_ = size_ == 0 ? 1 : 0;
	rem_calls_ = 1 + int(Random::exponential(calls_per_session_));
	rem_packets_ = 1 + int(Random::exponential(packets_per_call_));
	if (agent_)
		agent_->set_pkttype(PT_HTTP);
}

double Etsi_Traffic::next_interval(int& size)
{
	double t = Random::exponential(inter_arrival_time_);
	
	if (rem_packets_ == 0) {
		t = Random::exponential(reading_time_);
		rem_calls_--;
		rem_packets_ = 1 + int(Random::exponential(packets_per_call_));
	}
	
	if (rem_calls_ == 0) {
		t = Random::exponential(inter_session_time_);
		rem_calls_ = 1 + int(Random::exponential(calls_per_session_));
	}

	rem_packets_--;
	if (pareto_size_) {
		size_ = int(round(Random::pareto(81.5, 1.1)));
		if (size_ > 66666) {
			size_ = 66666;
		}
	}
	size = size_;
	return(t);	
}















