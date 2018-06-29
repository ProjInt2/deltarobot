#include <libdeltarobot/deltarobot.h>

namespace deltarobot
{
    
    robot_configuration::robot_configuration()
    {
        A1 = A2 = A3 = vec3(0,0,0);
        d_B1B3 = d_e = 0.0f;
    }
    
    robot_state::robot_state()
    {
        da = db = dc = 0.0f;
    }
    
    robot_state::robot_state(const robot_configuration &conf)
    {
        // todo minimum dislocation
        da = db = dc = 0.0f;
    }
    
    robot::robot(const deltarobot::robot_configuration& conf, deltarobot::robot_state st):
    m_conf(conf)
    {
        m_st = st;
    }
    
    
    static inline float ik_util_distance(vec3 a, vec3 b)
    {
        return (a-b).norm();
    }
    
    void robot::calculate_ik(float xe, float ye, float ze, float &q1, float &q2, float &q3)
    {
        
        float d_B1B3 = m_conf.d_B1B3;
        float d_e = m_conf.d_e;
        
        vec3 A1 = m_conf.A1,
             A2 = m_conf.A2,
             A3 = m_conf.A3;
        
        // Eq. 2: Calculate k_1, k_2, alpha
        
        //eq2
        float k_1 = xe;
        float k_2 = sqrt (pow(ye, 2) + pow(ze, 2));
        float alpha = 1/tan(-ye/ze);
        
        // Eq. 4: Calculate k_3
        
        float k_3 = atan( (xe - A1[0]) / ( cos(alpha)*(ze - A1[2]) - sin(alpha)*(ye-A1[1]) ) );
        
        // Eq. 5: Calculate B1
        
        float B1_x, B1_y, B1_z;
        
        B1_x = xe - d_B1B3 * cos(k_3) / 2.0f;
        B1_y = ye - d_B1B3 * sin(alpha) * sin(k_3) / 2.0f;
        B1_z = ze + d_B1B3 * cos(alpha) * sin(k_3) / 2.0f;
        
        vec3 B1(B1_x, B1_y, B1_z);
        
        // Eq. 6: Calculate B3
        
        float B3_x, B3_y, B3_z;
        
        B3_x = xe + d_B1B3 * cos(k_3) / 2.0f;
        B3_y = ye + d_B1B3 * sin(alpha) * sin(k_3) / 2.0f;
        B3_z = ze - d_B1B3 * cos(alpha) * sin(k_3) / 2.0f;
        
        vec3 B3(B3_x, B3_y, B3_z);
        
        
        // Eq. 7: Calculate B2
        
        float B2_x, B2_y, B2_z;
        
        B2_x = xe;
        B2_y = ye + d_e * cos(alpha);
        B2_z = ze + d_B1B3 * cos(alpha) * sin(k_3) / 2.0f;
        
        vec3 B2(B2_x, B2_y, B2_z);
        
        // Eq. 8: Calculate q1, q2, q3
        
        q1 = (A1.cross(B1)).norm();
        q2 = (A2.cross(B2)).norm();
        q3 = (A3.cross(B3)).norm();
        
        
    }
    
    /* -- do kenji
     
     //eq11
     D _0 = C_0**2+1;                                                                                             *
     D_1 = 2*C_0*C_1 + dB1B2 * sin(k_3) * C_0 – dB1B2* cos(k_3);
     D_2 = 0.25*dB1B2**2 + dB1B2*sin(k_3)*C_1-q_1**2;
     k_1 = (-D_1 + sqrt(D_1**2 – 4*D_0*D_2))/2*D_0;
     k_1 = (-D_1 - sqrt(D_1**2 – 4*D_0*D_2))/2*D_0;
     
     //eq12
     C_0 = (dB1B2*cos(k_3)-dA1A3) / dB1B2*sin(k_3);
     C_1 = (-dA1A3*dB1B2*cos(k_3)-q_3**2+dA1A3**2+q_1**2)/(2*dB1B2*sin(k_3));
     k_2 = C_0*k_1+C_1;;
     
     //Eq_13
     k_1 = (dB1B2 * cos(k_3) – dA1A3) / dB1B2*sin(k_3)
     k_2 = sqrt (((q_1**2+q_3**2+2*dA1A3*(k_1+0.5*dB1B2 *cos(k_3))-dA2A3**2 –(dB1B2**2)/2 – 2*k_1**2))/2;
     //eq14
     K_1 = -sin(k_3)*(z_e – z_a2);
     K_2 = sin(k_3)* (y_e – y_a2);
     K_2 = cos(k_3)* (x_e – x_a2);
     alpha = 2/tan((K_2**2 + sqrt(K_2**2+ K_1**2- K_3**2)/(K_3-K_1));
     alpha = 2/tan((K_2**2 - sqrt(K_2**2+ K_1**2- K_3**2)/(K_3-K_1));
     
     //eq15
     q22 = (k_1 – x_a2)**2 + k_2**2+d_e**2+y_a2**2+z_a2**2+2*(k_2*y_a3-d_3*z_a2)-2*(d_e*y_a2+k_2z_a3)*cos(alpha);
     */
    
    void robot::calculate_fk(float q1, float q2, float q3, float &xe, float &ye, float &ze)
    {
        
        if( q1 == q3  ) // sin(k3) == 0  TODO: verify assumption
        {
            // Equations 11 and 12: Calculate k1 and k2
            
            
        }
        else
        {
            // Equation 13: Calculate k1 and k2
            
        }
        
    }
    

    
}
