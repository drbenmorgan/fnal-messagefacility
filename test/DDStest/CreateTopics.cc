#include "ccpp_dds_dcps.h"
#include "ccpp_Chat.h"

#include <iostream>

using namespace DDS;
using namespace Chat;

int main (int argc, char *argv[])
{
  DomainParticipantFactory_var dpf;
  DomainParticipant_var        dp;
  DomainId_t               domain = NULL;
  ReturnCode_t             status;

  dpf = DomainParticipantFactory::get_instance();
  if (!dpf)
  {
    std::cout<<"Creating failed"<<std::endl;
    exit (-1);
  }

  dp = dpf->create_participant (
    domain,
    PARTICIPANT_QOS_DEFAULT,
    NULL,
    ANY_STATUS);
  if(!dp)
  {
    std::cout<<"Creating failed 2" <<std::endl;
    exit (-1);
  }

  status = dpf->delete_participant(dp.in());

  return 0;
}
