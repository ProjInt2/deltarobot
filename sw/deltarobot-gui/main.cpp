#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtSerialPort/QSerialPort>

#include "libdeltarobot/protocol.h"
#include "libdeltarobot/deltarobot.h"
#include <QVector3D>

class deltarobot_connection : public QObject
{
Q_OBJECT
Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)

public:
    deltarobot_connection()
    : m_sp()
    {
        m_sp.setBaudRate(115200);
        m_sp.setPortName("/dev/ttyACM0");
        m_sp.open(QIODevice::ReadWrite);
        
        if(m_sp.isOpen())
        {
            connect(&m_sp, &QSerialPort::readyRead, this, 
                    &deltarobot_connection::onReadyRead);
            
            emit connectedChanged(connected());
        }
        else
        {
            //perror("ttyACM0");
        }
    }
    
    int sendfunc( void *data, int bytes, void*me )
    {
        return ((deltarobot_connection*)me)->send_bytes(data, bytes);
    }
    
    int send_bytes( void *data, int bytes)
    {
        if(m_sp.isOpen())
        {
            m_sp.write((const char*)data, bytes);
            m_sp.flush();
        }
        else return -1;
    }
    
public slots:
    void onReadyRead()
    {
        QByteArray all = m_sp.readAll();
        dp_process(&m_pd, all.data(), all.size());
        
        protocol_msg msg;
        int res = dp_recv(&m_pd, &msg);
        
        if(!res)
        {
            if(!msg.checksum_ok)
                qDebug("checksum error\n");
            else
            // received a valid message
            switch(msg.opcode)
            {
                case DP_OPCODE_ACK:
                    emit ackReceived(msg.data_s? msg.data[0] : -1);
                case DP_OPCODE_NACK:
                    emit nackReceived(msg.data_s? msg.data[0] : -1);
                case DP_OPCODE_HOMING_REPL:
                    emit homingFinished();
                case DP_OPCODE_MOVE_REPL:
                    emit moveFinished();
                case DP_OPCODE_STATUS:
                    readStatus(msg);
                case DP_OPCODE_GENERAL_ERR:
                    emit generalError();
            }
        }
    }
    
    void sendAck(int opcode) 
    { 
        protocol_msg m;
        m.opcode = DP_OPCODE_ACK;
        m.data_s = 1;
        m.data[0] = opcode;
        
    }
    
    void sendNack(int opcode) { /*TODO STUB*/}
    void sendHome() { /*TODO STUB*/}
    void sendMove(float a, float b, float c) { /*TODO STUB*/}
    void askStatus() { /*TODO STUB*/}
    
public:
    bool connected() { return m_sp.isOpen(); }
    
signals:
    void connectedChanged( bool c );
    
    void ackReceived( char opcode );
    void nackReceived( char opcode );
    void homingFinished();
    void moveFinished();
    void statusReply(int status, bool stopa, float a, bool stopb, float b, bool stopc, float c);
    void generalError();
    
private:
    void readStatus(const protocol_msg &msg)
    {
        if(msg.data_s != 13)
        {
            printf("malformed status");
            return;
        }
        
        float a = *(float*)( msg.data + 0 );
        float b = *(float*)( msg.data + 4 );
        float c = *(float*)( msg.data + 8 );
        
        int st = msg.data[12] & 0b11;
        int stops = msg.data[12] >> 2;
        
        emit statusReply(st, stops&0b001, a, stops&0b010, b, stops&0b100, c);
    }

    QSerialPort m_sp;
    protocol_data m_pd;
};

QVector3D toQt(const deltarobot::vec3 &v)
{
    return QVector3D(v[0], v[1], v[2]);
}

deltarobot::vec3 toLib(const QVector3D &v)
{
    return deltarobot::vec3(v.x(), v.y(), v.z());
}

class deltarobot_model : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVector3D armAPos READ armAPos WRITE setArmAPos NOTIFY armAPosChanged)
    Q_PROPERTY(QVector3D armBPos READ armBPos WRITE setArmBPos NOTIFY armBPosChanged)
    Q_PROPERTY(QVector3D armCPos READ armCPos WRITE setArmCPos NOTIFY armCPosChanged)
    Q_PROPERTY(double dB1B3 READ dB1B3 WRITE setDB1B3 NOTIFY dB1B3Changed)
    Q_PROPERTY(double dE READ dE WRITE setDE NOTIFY dEChanged)
    
public:
    deltarobot_model()
    :m_robot(m_conf, m_state)
    {
        m_conf.A1 = deltarobot::vec3(0.12, 0.0, 0.10);
        m_conf.A2 = deltarobot::vec3(0.38, 0.0, 0.25);
        m_conf.A3 = deltarobot::vec3(0.12, 0.0, 0.40);
        
        m_conf.d_B1B3 = 0.15f;
        m_conf.d_e = 0.08f;
        
    }
    
    QVector3D armAPos()
    {
        return toQt(m_conf.A1);
    }
    
    void setArmAPos(QVector3D v)
    {
        if(v == armAPos())
            return;
        
        m_conf.A1 = toLib(v);
        emit armAPosChanged(v);
    }
    
    QVector3D armBPos()
    {
        return toQt(m_conf.A2);
    }
    
    void setArmBPos(QVector3D v)
    {
        if(v == armBPos())
            return;
        
        m_conf.A2 = toLib(v);
        emit armBPosChanged(v);
    }
    
    QVector3D armCPos()
    {
        return toQt(m_conf.A3);
    }
    
    void setArmCPos(QVector3D v)
    {
        if(v == armCPos())
            return;
        
        m_conf.A3 = toLib(v);
        emit armCPosChanged(v);
    }
    
    double dB1B3()
    {
        return m_conf.d_B1B3;
    }
    
    void setDB1B3(double v)
    {
        if(v == dB1B3())
            return;
        
        m_conf.d_B1B3 = v;
        emit dB1B3Changed(v);
    }
    
    double dE()
    {
        return m_conf.d_B1B3;
    }
    
    void setDE(double v)
    {
        if(v == dE())
            return;
        
        m_conf.d_e = v;
        emit dEChanged(v);
    }
    
public slots:
    
    QVector3D performIK( QVector3D desiredPos )
    {   
        float q1, q2, q3;
        m_robot.calculate_ik(desiredPos.x(), desiredPos.y(), desiredPos.z(), q1,q2,q3);
        return QVector3D(q1, q2, q3);
    }
    
    QVector3D performFK( QVector3D displacements )
    {
        return QVector3D(0, 0, 0);
    }
    
signals:
    void armAPosChanged(QVector3D v);
    void armBPosChanged(QVector3D v);
    void armCPosChanged(QVector3D v);
    void dB1B3Changed(double v);
    void dEChanged(double v);
    
private:
    deltarobot::robot_configuration m_conf;
    deltarobot::robot_state m_state;
    deltarobot::robot m_robot;
};


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // helpers 
    deltarobot_connection conn;
    deltarobot_model dmodel;
    
    QQmlApplicationEngine engine;
    
    engine.rootContext()->setContextProperty("conn", &conn);
    engine.rootContext()->setContextProperty("dmodel", &dmodel);
    
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    
    return app.exec();
}

#include "main.moc"
