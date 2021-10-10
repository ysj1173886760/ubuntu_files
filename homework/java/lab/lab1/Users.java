import java.util.Vector;

public class Users {
    static User admin = new User("admin", "123456");
    static Vector<Student> studentList = new Vector<Student>();
    static Vector<Teacher> teacherList = new Vector<Teacher>();

    public static void showStudents() {
        System.out.println("学号    名字    班级");
        for (int i = 0; i < Users.studentList.size(); i++) {
            Users.studentList.get(i).show();
        }
    }

    public static void showTeachers() {
        System.out.println("工号    名字    职称");
        for (int i = 0; i < Users.teacherList.size(); i++) {
            Users.teacherList.get(i).show();
        }
    }
}