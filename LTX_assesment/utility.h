#ifndef UTILITY_H
#define UTILITY_H
#include<vector>
#include"flight_data.h"

class utility
{
public:
    utility();

    static std::vector<flight_data> parse_to_flight_data(QString csv_data);
    static void sort_flightdata_by_time(std::vector<flight_data> &flight_data_vec);
    static QString export_to_json(const std::vector<flight_data>&flight_data_vec);

};

#endif // UTILITY_H
