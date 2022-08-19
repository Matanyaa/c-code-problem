#include "include.h"


bool_err encode_num(char *str);
bool is_data_image_full();

/*gets a string of array arguments, encoding it in data_image and returning NO_ERR.
	if unsuccessfull returns ERR*/
bool_err encode_arr(char *args)
{
	char *arg = strtok(args, ",");
	while (arg)
	{
		delete_initial_spaces(arg);
		delete_final_spaces(arg);
		if (encode_num(arg) == ERR)
		{
			return ERR;
		}
		arg = strtok(NULL, ",");
	}
	return NO_ERR;
}

/*encodes a string in the format "some string" into the data image and encodes null char at the end
	returns NO_ERR upon success, else returns ERR*/
bool_err encode_string(char *arg)
{
	delete_initial_spaces(arg);
	delete_final_spaces(arg);

	if (!is_valid_string(arg))
	{
		perr(STRING_ILLIGAL);
		return ERR;
	}

	*strrchr(arg, '\"') = '\0';
	arg++;

	do
	{
		DC++;
		data_image[num_ocupide_data_cells] = (unsigned int)(*arg);
		num_ocupide_data_cells++;
		arg++;
	} while (*arg);

	DC++;
	data_image[num_ocupide_data_cells] = '\0';
	num_ocupide_data_cells++;

	return NO_ERR;
}

/*encoding a struct in data_image and returning NO_ERR upon success, else returns ERR*/
bool_err encode_struct(char *args)
{
	char *arg;

	/*expecting number*/
	arg = strtok(args, ",");
	delete_initial_spaces(arg);
	delete_final_spaces(arg);
	if (!is_valid_num(arg))
	{
		perr(STRUCT_ILLIGAL);
		return ERR;
	}
	encode_num(arg);

	/*expecting string*/
	arg = strtok(NULL, "\0");
	delete_initial_spaces(arg);
	delete_final_spaces(arg);
	if (!is_valid_string(arg))
	{
		perr(STRUCT_ILLIGAL);
		return ERR;
	}
	encode_string(arg);

	return NO_ERR;
}


/*------------------------------private functions----------------------------------*/
/*gets a string represanting a number and encoding it in data_image and returns NO_ERR
	if the string isn't a valid integer returns ERR without encoding it*/
bool_err encode_num(char *str)
{
	int num;

	if (is_data_image_full())
	{
		perr(DATA_IMAGE_OVERFLOW);
		return ERR;
	}
	if (!is_valid_num(str))
	{
		perr(NUMBER_ILLIGAL);
		return ERR;
	}

	num = atoi(str);

	if (num < MIN_NUM || num > MAX_NUM)
	{
		perr(NUMBER_OUT_OF_BOUND);
		return ERR;
	}

	DC++;
	data_image[num_ocupide_data_cells] = num;
	num_ocupide_data_cells++;

	return NO_ERR;
}

/*true if data_image is full, else returns false*/
bool is_data_image_full()
{
	return num_ocupide_data_cells == SIZE_DATA_IMAGE;
}
