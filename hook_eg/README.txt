

应用层例如:

int nf_test(void)
{
	band_status;
	socklen_t len;
	len = sizeof(band_status);

	if((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) == -1)
		return -1;

	if(getsockopt(sockfd, IPPROTO_IP, SOE_BANDPING, (char*)&band_status, &len))
		return -1;

	return 0;

}

