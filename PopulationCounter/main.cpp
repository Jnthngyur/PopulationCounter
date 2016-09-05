// Author: Jonathan Gyurkovics
// Description: The purpose of this app is to take in a ascii based text file with the format of "Name","Birth Year","Death Year" and then calculate based
//				on this information which year the most number of these people were alive.

#define START_YEAR 1900
#define END_YEAR 2000
#define TOTAL_YEARS (END_YEAR - START_YEAR) + 1

#include <iostream>
#include <fstream>
#include <Windows.h>
#include <stdint.h>

// IN: char* string            - the string to parse
//     char delimiter          - the delimiter to check for
//     uint16_t& numSubStrings - reference to the number of sub strings found by the parser
// OUT: char**                 - the sub string array that was created from the original string and delimiter, nullptr if this failed
//
// Parses the incoming string looking for sub strings seperated by the passed in delimiter. numStrings will be set to the number of strings found.
// *NOTE* - The returned string array is dynamic memory in both the array and strings so they will all need to be cleaned up!!
char** StringParse(const char* string, char delimiter, uint16_t& numSubStrings)
{
	// Set the number of sub strings to 1
	numSubStrings = 1;

	// First check to see if we are getting an empty string. If so we should set the number of sub strings to 0 and return nullptr
	if (string == "")
	{
		numSubStrings = 0;
		return nullptr;
	}

	// First grab the length of our string
	uint16_t length = strlen(string);

	// Loop through the string to find how many sub strings we have
	uint16_t i = 0;
	for (; i <= length; i++)
	{
		// If we find our delimiter increase our substring count by one
		if (string[i] == delimiter)
			numSubStrings++;
	}

	// Now that we know our substring count we can create a string array and properly get the substrings
	char** subStrings = new char*[numSubStrings];

	// Now we can loop back through our string using a start index to keep track of were our sub string starts in the original string
	uint16_t startIndex = 0;
	uint16_t subStringIndex = 0;
	for (i = 0; i < (length + 1); i++)
	{
		// check to see if we found a delimiter or reached the end of the string
		if (string[i] == delimiter || i == length)
		{
			// Get the total number of characters this substring consists of and then create a char array based on that number
			uint16_t numChars = i - startIndex;
			subStrings[subStringIndex] = new char[numChars + 1];

			// Copy all the characters from the original string
			for (uint16_t character = 0; character < numChars; character++)
				subStrings[subStringIndex][character] = string[startIndex + character];
			subStrings[subStringIndex][numChars] = '\0';

			// Move our start index to the next index of the string since we know we are currently on a delimiter and increment our sub string
			// counter since we now have finished with this sub string
			startIndex = i + 1;
			subStringIndex++;
		}
	}

	return subStrings;
}

// IN: int argc    - number of arguments sent to the program
//     char** argv - string based arguments sent to the program
// OUT: void
//
// Main entry point to the program
void main(int argc, char** argv)
{
	// Create a list of the total years we are working with and intialize them to 0
	uint32_t numAlive[TOTAL_YEARS] = {};

	// We want to loop through each file that was dragged onto the exe incase we are doing multiple lists
	for (uint16_t i = 1; i < argc; i++)
	{
		// If so try to open the file
		std::ifstream data;
		data.open(argv[i], std::ios_base::in);

		// Check if we didn't open a file
		if (!data.is_open())
			continue;

		char buffer[200];
		// Loop through until we hit the end of the file
		while (!data.eof())
		{
			// Read a line from the file
			data.getline(buffer, 200, '\n');

			// Split each line based on ' ' as the delimiter
			uint16_t numSubStrings = 0;
			char** splitString = StringParse(buffer, ',', numSubStrings);

			// If we got a bad string we want to skip this line
			if (numSubStrings < 3)
			{
				// If we got a valid pointer we need to clean up the data
				if (splitString)
				{
					for (uint16_t string = 0; string < numSubStrings; string++)
						delete[] splitString[string];

					delete[] splitString;
				}

				continue;
			}

			// If we have a valid string we can now work with the data to populate our data pool
			// Since we are only interested about the birth and death years, we can skip over the name sub string
			uint16_t birthYear = max(atoi(splitString[1]), START_YEAR);		// Using max keeps the birth year at or above the start year so we only get the years we want
			uint16_t deathYear = min(atoi(splitString[2]), END_YEAR);		// Using min we can do the same to death year but keeping it at or below the end year

			// If our years don't fall in line with our set period we are looking for or the death year is before the birth year, skip over them
			if (birthYear > END_YEAR || deathYear < START_YEAR || deathYear < birthYear)
			{
				for (uint16_t string = 0; string < numSubStrings; string++)
					delete[] splitString[string];

				delete[] splitString;

				continue;
			}

			// If our birth and death years are fine we can now feed this information to the data pool.
			// We can calculate the number of years the person was alive and from there increment the corresponding year
			uint16_t totalYearsAlive = deathYear - birthYear;
			for (uint16_t year = 0; year <= totalYearsAlive; year++)
				numAlive[(birthYear - START_YEAR) + year]++;

			// Now we can clean up our strings and continue
			for (uint16_t string = 0; string < numSubStrings; string++)
				delete[] splitString[string];

			delete[] splitString;
		}

		// Close our file
		data.close();
	}

	// Loop through each year and find which one has the most people alive
	uint16_t yearMostAlive = 0;
	for (uint16_t i = 0; i < TOTAL_YEARS; i++)
	{
		if (numAlive[i] > numAlive[yearMostAlive])
			yearMostAlive = i;
	}

	// Display all the information
	std::cout << "Year:    Alive:\n";

	for (uint16_t i = 0; i < TOTAL_YEARS; i++)
		std::cout << START_YEAR + i << "     " << numAlive[i] << '\n';

	std::cout << "\nThe year the most people were alive was " << START_YEAR + yearMostAlive << " and the total people alive was " << numAlive[yearMostAlive] << ".\n";

	system("PAUSE");
}