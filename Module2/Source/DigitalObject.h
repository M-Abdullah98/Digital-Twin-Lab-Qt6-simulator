#ifndef DIGITALOBJECT_H
#define DIGITALOBJECT_H

// FIX: File renamed from "BC(digitalObject).h" to "DigitalObject.h".
//   Parentheses in filenames are non-standard and cause issues with many
//   build systems and compilers.

#include <string>
#include <iostream>

// Abstract Base Class
class DigitalObject {
protected:
    int         objectID;  // Unique identifier
    std::string name;      // Object name
    std::string status;    // Current status (e.g., "Active", "Inactive")
    // FIX: Spec lists creationDate as an attribute — added.
    std::string creationDate;

public:
    DigitalObject(int id,
                  const std::string& objName,
                  const std::string& objStatus,
                  const std::string& date = "N/A")
        : objectID(id), name(objName), status(objStatus), creationDate(date) {}

    // FIX: Virtual destructor is essential for correct polymorphic deletion.
    virtual ~DigitalObject() = default;

    // Pure Virtual Functions
    virtual void  displayInfo()      const = 0;
    virtual void  updateState()            = 0;
    virtual float calculateHealth()  const = 0;

    // Getters
    int         getObjectID()      const { return objectID; }
    std::string getName()          const { return name; }
    std::string getStatus()        const { return status; }
    std::string getCreationDate()  const { return creationDate; }

    // Setters
    void setName(const std::string& newName)     { name   = newName; }
    void setStatus(const std::string& newStatus) { status = newStatus; }
};

#endif // DIGITALOBJECT_H
