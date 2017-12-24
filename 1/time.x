/* time.x */
program Time_PROG{
	version Time_Vers{
		int Get_time(void) = 1;
		void Delay_time(int) = 2;
	} = 1;
} = 0x31234567;
