
static int hi_3531_vin_init()
{
	return -1;
}

static int hi_3531_vin_deinit()
{
	return -1;
}

static int hi_3531_vin_open(int chn, int audio_id)
{
	
	return -1;
}

static int hi_3531_vin_is_open(int chn)
{
	return 1;
}

static int hi_3531_vin_close(int chn)
{

	return 1;
}

lpJDK_VIN_IFACE g_jdkHi3531VinIface = 
{
	hi_3531_vin_init,
	hi_3531_vin_open,
	hi_3531_vin_is_open,
	hi_3531_vin_close,
	hi_3531_vin_deinit,
};

