#pragma once

#include "eigen3/Eigen/Dense"

namespace deltarobot
{
    
    typedef Eigen::Vector2f vec2;
    typedef Eigen::Vector3f vec3;
    typedef Eigen::Vector4f vec4;
    typedef Eigen::Quaternionf quat;
    
    struct robot_configuration
    {
        robot_configuration();
        
        vec3 A1, A2, A3;   // joint positions (in absolute coordinate space)
        float d_B1B3, d_e; // platform parameters
    };
    
    struct robot_state
    {
        robot_state();
        robot_state(const robot_configuration &conf); // default robot state
        
        float da;
        float db;
        float dc;
    };
    
    class robot
    {
    public:
        robot(const robot_configuration& conf, robot_state st); 
        
        void calculate_ik(float xe, float ye, float ze, float &q1, float &q2, float &q3);
        void calculate_fk(float q1, float q2, float q3, float &xe, float &ye, float &ze);
        
    private:
        const robot_configuration &m_conf;
        robot_state m_st;
    };

}
