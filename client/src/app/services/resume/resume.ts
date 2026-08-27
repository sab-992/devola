import { Injectable, PLATFORM_ID, inject } from '@angular/core';
import { Observable } from 'rxjs';
import { Resume } from '@models/resume';
import { isPlatformBrowser } from '@angular/common';
import { HttpService } from '@services/http/http';
import { environment } from '@environments/environment';


const PDF_WORKER_SRC = 'assets/pdf.worker.min.mjs';
const SUPPORTED_EXTENSIONS = ['pdf', 'txt', 'md'] as const;

@Injectable({ providedIn: 'root' })
export class ResumeService {
    private readonly m_http = inject(HttpService);
    private readonly platformID = inject(PLATFORM_ID);

    public async extractText(file: File): Promise<string> {
        const extension = this.getExtension(file.name);

        let content: Promise<string>;
        switch (extension) {
            case 'txt':
            case 'md':
                content = this.extractPlainText(file);
                break;
            case 'pdf':
                content =  this.extractPdfText(file);
                break;
            default:
                throw new Error(`Unsupported file type ".${extension}". Upload a ${SUPPORTED_EXTENSIONS.join(', ')} file.`);
        }

        return content;
    }

    public addResume(payload: Resume): Observable<Resume> {
        return this.m_http.post<Resume, Resume>(this.buildPath("/resumes"), payload);
    }

    public deleteResume(tag: string) {
        return this.m_http.delete(`${this.buildPath("/resumes")}/${tag}`);
    }

    public fetchResumes() : Observable<Resume[]> {
        return this.m_http.get<Resume[]>(this.buildPath("/resumes"));
    }

    public updateResume(tag: string, skills: string[]) {
        return this.m_http.patch<Resume, string[]>(`${this.buildPath("/resumes")}/${tag}`, skills);
    }

    private async extractPdfText(file: File): Promise<string> {
        if (isPlatformBrowser(this.platformID)) {
            const pdfjsLib = await import('pdfjs-dist');
            pdfjsLib.GlobalWorkerOptions.workerSrc = PDF_WORKER_SRC;

            const buffer = await file.arrayBuffer();
            const pdf = await pdfjsLib.getDocument({ data: buffer }).promise;

            const pageTexts: string[] = [];
            for (let pageNumber = 1; pageNumber <= pdf.numPages; pageNumber++) {
                const page = await pdf.getPage(pageNumber);
                const content = await page.getTextContent();
                const pageText = content.items.map((item) => ('str' in item ? item.str : '')).join(' ');
                pageTexts.push(pageText);
            }

            return pageTexts.join('\n\n').trim();
        }

        return new Promise(()=>{});
    }

    private async extractPlainText(file: File): Promise<string> {
        return (await file.text()).trim();
    }

    private getExtension(fileName: string): string {
        return fileName.split('.').pop()?.toLowerCase() ?? '';
    }

    private buildPath(path: string) {
        return `${environment.apiUrl}/rss${path}`;
    }
}