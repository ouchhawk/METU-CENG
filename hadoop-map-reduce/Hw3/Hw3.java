import java.io.IOException;
import java.util.StringTokenizer;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.DoubleWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;

public class Hw3 {

   public static class capMapper extends Mapper<Object, Text, Text, IntWritable> {

      private final static IntWritable one = new IntWritable(1);
      private Text word = new Text();
      private int i = 0;

      public void map(Object key, Text value, Context context) throws IOException, InterruptedException {
         StringTokenizer itr = new StringTokenizer(value.toString(), "\n ");

         while (itr.hasMoreTokens()) {
            String sID = itr.nextToken();
            String cID = itr.nextToken();
            String grade = itr.nextToken();

            word.set(cID);
            context.write(word, one);
         }

      }
   }

   public static class passMapper extends Mapper<Object, Text, Text, IntWritable> {

      private final static IntWritable one = new IntWritable(1);
      private Text word = new Text();
      private int i = 0;

      public void map(Object key, Text value, Context context) throws IOException, InterruptedException {
         StringTokenizer itr = new StringTokenizer(value.toString(), "\n ");

         while (itr.hasMoreTokens()) {
            String sID = itr.nextToken();
            String cID = itr.nextToken();
            String grade = itr.nextToken();

            if (Integer.parseInt(grade) >= 60 && Integer.parseInt(grade) > 0) {
               word.set(sID);
               context.write(word, one);
            }
         }

      }
   }

   public static class avgMapper extends Mapper<Object, Text, Text, DoubleWritable> {

      private final static DoubleWritable one = new DoubleWritable(1.0);
      private Text word = new Text();
      private int i = 0;

      public void map(Object key, Text value, Context context) throws IOException, InterruptedException {
         StringTokenizer itr = new StringTokenizer(value.toString(), "\n ");

         while (itr.hasMoreTokens()) {
            String sID = itr.nextToken();
            String cID = itr.nextToken();
            String grade = itr.nextToken();

            if (Double.parseDouble(grade) > 0 && Double.parseDouble(grade) < 101) {
               DoubleWritable two = new DoubleWritable(Double.parseDouble(grade));
               word.set(sID);
               context.write(word, two);
            }
         }

      }
   }

   public static class twolistMapperPass extends Mapper<Object, Text, Text, DoubleWritable> {

      private final static DoubleWritable one = new DoubleWritable(1.0);
      private Text word = new Text();
      private int i = 0;

      public void map(Object key, Text value, Context context) throws IOException, InterruptedException {
         StringTokenizer itr = new StringTokenizer(value.toString(), "\n ");

         while (itr.hasMoreTokens()) {
            String sID = itr.nextToken();
            String cID = itr.nextToken();
            String grade = itr.nextToken();

            if (Double.parseDouble(grade) > 59 && Double.parseDouble(grade) < 101) {
               DoubleWritable two = new DoubleWritable(Double.parseDouble(grade));
               word.set(cID);
               context.write(word, two);
            }
         }
      }
   }

   public static class twolistMapperFail extends Mapper<Object, Text, Text, DoubleWritable> {

      private final static DoubleWritable one = new DoubleWritable(1.0);
      private Text word = new Text();
      private int i = 0;

      public void map(Object key, Text value, Context context) throws IOException, InterruptedException {
         StringTokenizer itr = new StringTokenizer(value.toString(), "\n ");

         while (itr.hasMoreTokens()) {
            String sID = itr.nextToken();
            String cID = itr.nextToken();
            String grade = itr.nextToken();

            if (Double.parseDouble(grade) > 0 && Double.parseDouble(grade) < 60) {
               DoubleWritable two = new DoubleWritable(Double.parseDouble(grade));
               word.set(cID);
               context.write(word, two);
            }
         }
      }
   }
   public static class IntSumReducer extends Reducer<Text, IntWritable, Text, IntWritable> {
      private IntWritable result = new IntWritable();

      public void reduce(Text key, Iterable<IntWritable> values, Context context)
            throws IOException, InterruptedException {
         int sum = 0;
         for (IntWritable val : values) {
            sum += val.get();
         }
         result.set(sum);
         context.write(key, result);
      }
   }

   public static class DoubleSumReducer extends Reducer<Text, DoubleWritable, Text, DoubleWritable> {
      private DoubleWritable result = new DoubleWritable();

      public void reduce(Text key, Iterable<DoubleWritable> values, Context context)
            throws IOException, InterruptedException {
         double sum = 0;
         double count = 0;
         for (DoubleWritable val : values) {
            sum += val.get();
            count = count + 1.0;
         }
         result.set(sum / count);
         context.write(key, result);
      }
   }

   public static class DoubleSumReducer2 extends Reducer<Text, DoubleWritable, Text, DoubleWritable> {
            
      private DoubleWritable result = new DoubleWritable();

      public void reduce(Text key, Iterable<DoubleWritable> values, Context context)
            throws IOException, InterruptedException {
         double sum = 0;
         double count = 0;
         for (DoubleWritable val : values) {
            sum += val.get();
            count = count + 1.0;
         }
         result.set(sum / count);
         context.write(key, result);
      }

   }

   

   public static void main(String[] args) throws Exception {
      if (args[0].equals("cap")) {
         Configuration conf = new Configuration();
         Job job = Job.getInstance(conf, "word count");
         job.setJarByClass(Hw3.class);
         job.setMapperClass(capMapper.class);
         job.setCombinerClass(IntSumReducer.class);
         job.setReducerClass(IntSumReducer.class);
         job.setOutputKeyClass(Text.class);
         job.setOutputValueClass(IntWritable.class);
         FileInputFormat.addInputPath(job, new Path(args[1]));
         FileOutputFormat.setOutputPath(job, new Path(args[2]));
         System.exit(job.waitForCompletion(true) ? 0 : 1);
      } else if (args[0].equals("pass")) {
         Configuration conf = new Configuration();
         Job job = Job.getInstance(conf, "word count");
         job.setJarByClass(Hw3.class);
         job.setMapperClass(passMapper.class);
         job.setCombinerClass(IntSumReducer.class);
         job.setReducerClass(IntSumReducer.class);
         job.setOutputKeyClass(Text.class);
         job.setOutputValueClass(IntWritable.class);
         FileInputFormat.addInputPath(job, new Path(args[1]));
         FileOutputFormat.setOutputPath(job, new Path(args[2]));
         System.exit(job.waitForCompletion(true) ? 0 : 1);
      } else if (args[0].equals("avg")) {
         Configuration conf = new Configuration();
         Job job = Job.getInstance(conf, "word count");
         job.setJarByClass(Hw3.class);
         job.setMapperClass(avgMapper.class);
         job.setCombinerClass(DoubleSumReducer.class);
         job.setReducerClass(DoubleSumReducer.class);
         job.setOutputKeyClass(Text.class);
         job.setOutputValueClass(DoubleWritable.class);
         FileInputFormat.addInputPath(job, new Path(args[1]));
         FileOutputFormat.setOutputPath(job, new Path(args[2]));
         System.exit(job.waitForCompletion(true) ? 0 : 1);
      } else if (args[0].equals("twolist")) {
         Configuration conf = new Configuration();
         Job job = Job.getInstance(conf, "word count");

         //job.setNumReduceTasks(2);
         job.setJarByClass(Hw3.class);
         job.setMapperClass(twolistMapperPass.class);
         job.setCombinerClass(DoubleSumReducer2.class);
         job.setReducerClass(DoubleSumReducer2.class);
         job.setOutputKeyClass(Text.class);
         job.setOutputValueClass(DoubleWritable.class);
         FileInputFormat.addInputPath(job, new Path(args[1]));
         FileOutputFormat.setOutputPath(job, new Path(args[2]));

         System.exit(job.waitForCompletion(true) ? 0 : 1);

      } else {
         System.out.println("Default");
      }
   }
}