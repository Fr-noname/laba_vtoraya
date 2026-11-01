#include <iostream>
#include <vector>
#include <string>
#include <fstream>

struct Student {
    std::string fio;
    std::string group;
    int recordBookNumber;
    std::vector<int> grades;

    void display() const {
        std::cout << "FIO: " << fio << ", Group: " << group
                  << ", zachetka: " << recordBookNumber << ", ocenki: ";
        for (int grade : grades) {
            std::cout << grade << " ";
        }
        std::cout << std::endl;
    }
};

void writeStudentsText(const std::vector<Student>& students, const std::string& filename) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Error " << filename << std::endl;
        return;
    }

    for (const auto& student : students) {
        outFile << student.fio << "\n";
        outFile << student.group << "\n";
        outFile << student.recordBookNumber << "\n";
        for (int grade : student.grades) {
            outFile << grade << " ";
        }
        outFile << "\n";
    }
    outFile.close();
}

std::vector<Student> readStudentsText(const std::string& filename) {
    std::vector<Student> students;
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        std::cerr << "Error " << filename << std::endl;
        return students;
    }

    std::string line;
    while (std::getline(inFile, line)) {
        Student student;
        student.fio = line;
        std::getline(inFile, student.group);
        inFile >> student.recordBookNumber;
        inFile.ignore();

        int grade;
        for (int i = 0; i < 4; ++i) {
            inFile >> grade;
            student.grades.push_back(grade);
        }
        inFile.ignore();
        students.push_back(student);
    }
    inFile.close();
    return students;
}

void writeStudentsBinary(const std::vector<Student>& students, const std::string& filename) {
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile.is_open()) {
        std::cerr << "Error " << filename << std::endl;
        return;
    }

    for (const auto& student : students) {
        size_t fioLen = student.fio.length();
        outFile.write(reinterpret_cast<const char*>(&fioLen), sizeof(fioLen));
        outFile.write(student.fio.c_str(), fioLen);

        size_t groupLen = student.group.length();
        outFile.write(reinterpret_cast<const char*>(&groupLen), sizeof(groupLen));
        outFile.write(student.group.c_str(), groupLen);

        outFile.write(reinterpret_cast<const char*>(&student.recordBookNumber), sizeof(student.recordBookNumber));

        for (int grade : student.grades) {
            outFile.write(reinterpret_cast<const char*>(&grade), sizeof(grade));
        }
    }
    outFile.close();
}

std::vector<Student> readStudentsBinary(const std::string& filename) {
    std::vector<Student> students;
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile.is_open()) {
        std::cerr << "Error " << filename << std::endl;
        return students;
    }

    while (inFile.peek() != EOF) {
        Student student;

        size_t fioLen;
        inFile.read(reinterpret_cast<char*>(&fioLen), sizeof(fioLen));
        student.fio.resize(fioLen);
        inFile.read(&student.fio[0], fioLen);

        size_t groupLen;
        inFile.read(reinterpret_cast<char*>(&groupLen), sizeof(groupLen));
        student.group.resize(groupLen);
        inFile.read(&student.group[0], groupLen);

        inFile.read(reinterpret_cast<char*>(&student.recordBookNumber), sizeof(student.recordBookNumber));

        student.grades.resize(4);
        for (int i = 0; i < 4; ++i) {
            inFile.read(reinterpret_cast<char*>(&student.grades[i]), sizeof(student.grades[i]));
        }
        students.push_back(student);
    }
    inFile.close();
    return students;
}


int main() {
    std::string mode;
    std::cin >> mode;
    int n;
    std::cin >> n;

    std::vector<Student> students(n);
    students[0] = {"Ivan Ivanovich Ivanov", "11", 1, {5, 4, 5, 3}};
    students[1] = {"Peter Petrovich Petrov", "12", 12, {3, 3, 4, 5}};
    students[2] = {"Povsikakiy Povsikakievich Povsikakiev", "13", 13, {5, 5, 5, 4}};

    if (mode == "text") {
        std::cout << "text" << std::endl;
        writeStudentsText(students, "students.txt");
        std::vector<Student> readStudents = readStudentsText("students.txt");
        for (const auto& s : readStudents) {
            s.display();
        }
    } else if (mode == "binary") {
        std::cout << "bin" << std::endl;
        writeStudentsBinary(students, "students.bin");
        std::vector<Student> readStudents = readStudentsBinary("students.bin");
        for (const auto& s : readStudents) {
            s.display();
        }
    } else {
        std::cerr << "Error" << std::endl;
        return 1;
    }

    std::cout << "\nsravnenie" << std::endl;
    std::ifstream textFile("students.txt", std::ios::binary | std::ios::ate);
    std::ifstream binFile("students.bin", std::ios::binary | std::ios::ate);

    if (textFile.is_open()) {
        std::cout << "students.txt " << textFile.tellg() << " bayt" << std::endl;
        textFile.close();
    }
    if (binFile.is_open()) {
        std::cout << "students.bin " << binFile.tellg() << " bayt" << std::endl;
        binFile.close();
    }
    return 0;
}
