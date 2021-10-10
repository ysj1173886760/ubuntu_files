import java.util.Scanner;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.FileInputStream;
import java.util.Vector;


public class Courses {
    static Vector<Course> clist = new Vector<Course>();

    public static void addCourse() {
        if (Courses.clist.size() < 1) {
            Courses.clist.add(Course.inputCourse(1));
        } else {
            Courses.clist.add(Course.inputCourse(Courses.clist.size() + 1));
        }
    }

    public static void addCourses() {
        int i = 1;
        Scanner sc = new Scanner(System.in);
        while (true) {
            System.out.println("请输入第" + (i++) + "门课程信息");
            Courses.addCourse();
            System.out.print("是否继续添加课程？ y/n");
            if (sc.next().equals("n")) {
                break;
            }
        }
    }

    public static void showCourses() {
        System.out.println("编号    课程    类型    教师    选课人数");
        for (int i = 0; i < Courses.clist.size(); i++) {
            ((Course)(Courses.clist.get(i))).show();
        }
    }

    public static void SortCourseList() {
        Course c, k, m;
        for (int i = 0; i < Courses.clist.size(); i++) {
            for (int j = i + 1; j < Courses.clist.size(); j++) {
                k = (Course)(Courses.clist.get(j));
                c = (Course)(Courses.clist.get(i));
                m = c;
                if (c.stuNum < k.stuNum) {
                    Courses.clist.set(i, k);
                    Courses.clist.set(j, m);
                }
            }
        }
    }

    public static void SearchCourseByTeacher() {
        System.out.println("请输入需要查找的教师名称：");
        Scanner sc = new Scanner(System.in);
        String name = sc.next();
        for (int i = 0; i < Courses.clist.size(); i++) {
            Course c = ((Course)(Courses.clist.get(i)));
            if (c.teacher.equals(name)) {
                c.show();
            }
        }
    }

    public static void deleteCourse() {
        System.out.println("请输入需要删除的课程名称：");
        Scanner sc = new Scanner(System.in);
        String name = sc.next();
        for (int i = 0; i < Courses.clist.size(); i++) {
            if (Courses.clist.get(i).name.equals(name)) {
                Courses.clist.remove(i);
                break;
            }
        }
    }

    public static void saveCourse() {
        File file = new File("./Courses.txt");
        try {
            if (!file.exists()) {
                file.createNewFile();
            }
            BufferedWriter out = new BufferedWriter(new FileWriter(file.getAbsoluteFile()));
            for (int i = 0; i < Courses.clist.size(); i++) {
                out.write(((Course)(Courses.clist.get(i))).toString());
            }
            out.flush();
            out.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static void readCourse() {
        try {
            BufferedReader br = new BufferedReader(new InputStreamReader(new FileInputStream("./Courses.txt")));
            String data = null;
            while ((data = br.readLine()) != null) {
                String[] ps = data.split(" ");
                int id = Integer.parseInt(ps[0]);
                String name = ps[1];
                int type = Integer.parseInt(ps[2]);
                int stuNum = Integer.parseInt(ps[3]);
                String teacher = ps[4];
                if (type == 0) {
                    int credit = Integer.parseInt(ps[5]);
                    Courses.clist.add(new RequiredCourse(id, name, type, stuNum, teacher, credit));
                } else {
                    int maxStu = Integer.parseInt(ps[5]);
                    Courses.clist.add(new OptionalCourse(id, name, type, stuNum, teacher, maxStu));
                }
            }
            br.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
