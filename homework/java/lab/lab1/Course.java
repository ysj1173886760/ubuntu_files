import java.util.Scanner;

public class Course {
    int id;
    String name;
    int type;
    int stuNum;
    String teacher;

    public Course(int id, String name, int type, int stuNum, String teacher) {
        this.id = id;
        this.name = name;
        this.type = type;
        this.stuNum = stuNum;
        this.teacher = teacher;
    }

    public String toString() {
        return id + " " + name + " " + type + " " + stuNum + " " + teacher;
    }

    public Course() {}

    public static Course inputCourse(int id) {
        Scanner sc = new Scanner(System.in);
        System.out.println("请输入课程名：");
        String name = sc.next();
        System.out.println("请输入课程类型（0必修，1选修）：");
        int type = sc.nextInt();
        System.out.println("请输入上课教师：");
        String teacher = sc.next();
        System.out.println("请输入选课人数：");
        int stuNum = sc.nextInt();
        Course c;
        if (type == 0) {
            System.out.println("请输入学分：");
            int credit = sc.nextInt();
            c = new RequiredCourse(id, name, type, stuNum, teacher, credit);
        } else {
            System.out.println("请输入最大选课人数：");
            int maxStu = sc.nextInt();
            c = new OptionalCourse(id, name, type, stuNum, teacher, maxStu);
        }
        return c;
    }

    public void show() {
        System.out.print(id);
        System.out.print(" " + name);
        switch(type) {
        case 0:
            System.out.println(" 必修");
            break;
        case 1:
            System.out.println(" 选修");
            break;
        default:
            System.out.println(" 其他");
        }
    }
}