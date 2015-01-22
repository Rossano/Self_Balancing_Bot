

class complementary_filter
{
  private:
    float last_x_angle;
    float x_angle;
    float last_y_angle;
    float y_angle;
    float last_z_angle;
    float z_angle;
    float last_x_gyro;
    float x_gyro;
    float last_y_gyro;
    float y_gyro;
    float last_z_gyro;
    float z_gyro;
    float last_time;
    float t_now;
    void update_angle_data(unsigned long, float, float, float, float, float, float);
    
  public:
    complementary_filter();
    ~complementary_filter();
    void update_filter(unsigned long, float, float, float, float, float, float);
    float alpha;
    float beta;
    float get_x_angle();
    float get_y_angle();
    float get_z_angle();
    float get_x_gyro();
    float get_y_gyro();
    float get_z_gyro();
};

    
