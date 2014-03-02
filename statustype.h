#ifndef STATUSTYPE_H
#define STATUSTYPE_H
#include <QString>



class StatusType{
public:
    enum Status {STATUS_SUCCESS, STATUS_FAILURE, STATUS_UST, PASSWORD_REQ, BAD_UUID, UNKNOWN};
    static Status getStatus(QString msg)
    {
        bool ok = false;
        int value = msg.toInt(&ok);
        if(!ok)
            return UNKNOWN;
        return *((Status*)(&value));
    }
};

#endif // STATUSTYPE_H
