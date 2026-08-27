import { DecimalPipe } from '@angular/common';
import { COMMA, ENTER } from '@angular/cdk/keycodes';
import { Component, DestroyRef, OnInit, computed, inject, signal } from '@angular/core';
import { takeUntilDestroyed } from '@angular/core/rxjs-interop';
import { MatButtonModule } from '@angular/material/button';
import { MatChipInputEvent, MatChipsModule } from '@angular/material/chips';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatIconModule } from '@angular/material/icon';
import { MatInputModule } from '@angular/material/input';
import { MatProgressSpinnerModule } from '@angular/material/progress-spinner';

import { ResumeService } from '@services/resume/resume';
import { HeaderComponent } from "@components/header/header";
import { Resume } from '@models/resume';


@Component({
    selector: 'app-resume',
    standalone: true,
    imports: [
        DecimalPipe,
        MatButtonModule,
        MatChipsModule,
        MatFormFieldModule,
        MatIconModule,
        MatInputModule,
        MatProgressSpinnerModule,
        HeaderComponent
    ],
    templateUrl: './resume.html',
    styleUrl: './resume.scss',
})
export class ResumePage implements OnInit {
    private readonly m_resumeService = inject(ResumeService);
    private readonly m_destroyRef = inject(DestroyRef);

    readonly separatorKeyCodes = [ENTER, COMMA];
    readonly selectedFile = signal<File | null>(null);
    readonly dragOver = signal(false);
    readonly extracting = signal(false);
    readonly extractError = signal<string | null>(null);
    private readonly extractedText = signal<string | null>(null);
    readonly characterCount = computed(() => this.extractedText()?.length ?? 0);
    readonly tag = signal('');
    readonly skills = signal<string[]>([]);
    readonly saving = signal(false);
    readonly saveError = signal<string | null>(null);
    readonly saveSuccess = signal(false);
    readonly resumes = signal<Resume[]>([]);
    readonly loadingResumes = signal(false);
    readonly resumesError = signal<string | null>(null);
    readonly editingResumeTag = signal<string | null>(null);
    readonly editSkills = signal<string[]>([]);
    readonly editSkillsSaving = signal(false);
    readonly editSkillsError = signal<string | null>(null);
    readonly confirmDeleteTag = signal<string | null>(null);
    readonly deletingTag = signal<string | null>(null);
    readonly canSave = computed(() => !!this.selectedFile() &&
                                      !!this.extractedText() &&
                                      this.tag().trim().length > 0 &&
                                      !this.extracting() &&
                                      !this.saving());

    constructor() { }

    public ngOnInit(): void {
        this.loadResumes()
    }

    public addSkill(event: MatChipInputEvent): void {
        const value = event.value.trim();

        if (value && !this.skills().includes(value))
            this.skills.update((list) => [...list, value]);

        event.chipInput?.clear();
    }

    public addEditSkill(event: MatChipInputEvent): void {
        const value = event.value.trim();

        if (value && !this.editSkills().includes(value))
            this.editSkills.update((list) => [...list, value]);

        event.chipInput?.clear();
    }

    public cancelDelete(): void {
        this.confirmDeleteTag.set(null);
    }

    public cancelEditSkills(): void {
        this.editingResumeTag.set(null);
        this.editSkillsError.set(null);
    }

    public confirmDelete(tag: string): void {
        this.deletingTag.set(tag);
        this.m_resumeService.deleteResume(tag)
                            .pipe(takeUntilDestroyed(this.m_destroyRef))
                            .subscribe({ next: () => { this.resumes.update((list) => list.filter((r) => r.tag !== tag));
                                                       this.deletingTag.set(null);
                                                       this.confirmDeleteTag.set(null); },
                                         error: () => { this.deletingTag.set(null); }});
    }

    public clearFile(): void {
        this.selectedFile.set(null);
        this.extractedText.set(null);
        this.extractError.set(null);
    }

    public loadResumes() {
        this.loadingResumes.set(true);
        this.resumesError.set(null);
        this.m_resumeService.fetchResumes()
                            .pipe(takeUntilDestroyed(this.m_destroyRef))
                            .subscribe({ next: (resumes: Resume[]) => { this.resumes.update(() => { return resumes.map((resume)=> { return {...resume, skills: resume.skills.filter((skill)=>skill.length > 0) }; }); });
                                                                        this.loadingResumes.set(false); },
                                         error: () => { this.loadingResumes.set(false);
                                                        this.resumesError.set('Could not load resumes. Please try again.'); }});
    }

    public onDragLeave(): void {
        this.dragOver.set(false);
    }

    public onDragOver(event: DragEvent): void {
        event.preventDefault();
        this.dragOver.set(true);
    }

    public onFileDropped(event: DragEvent): void {
        event.preventDefault();
        this.dragOver.set(false);
        const file = event.dataTransfer?.files?.[0];

        if (file)
            this.loadFile(file);
    }

    public onFileSelected(event: Event): void {
        const input = event.target as HTMLInputElement;
        const file = input.files?.[0];

        if (file)
            this.loadFile(file);

        input.value = '';
    }

    public removeSkill(skill: string): void {
        this.skills.update((list) => list.filter((s) => s !== skill));
    }

    public requestDelete(tag: string): void {
        this.confirmDeleteTag.set(tag);
    }

    public save(): void {
        const content = this.extractedText();
        const file = this.selectedFile();

        if (!this.canSave() || !content || !file)
            return;

        this.saving.set(true);
        this.saveError.set(null);
        this.saveSuccess.set(false);

        this.m_resumeService.addResume({ tag: this.tag().trim(),
                                         skills: this.skills(),
                                         content,
                                         last_updated_at: -1 })
                            .pipe(takeUntilDestroyed(this.m_destroyRef))
                            .subscribe({ next: (newResume: Resume) => { this.saving.set(false);
                                                                        this.saveSuccess.set(true);
                                                                        this.resumes.update((list) => [...list, newResume]);
                                                                        this.resetForm(); },
                                         error: () => { this.saving.set(false);
                                                        this.saveError.set('Could not save this resume. Please try again.'); }});
    }

    public saveEditSkills(): void {
        const tag = this.editingResumeTag();
        if (!tag)
            return;

        this.editSkillsSaving.set(true);
        this.editSkillsError.set(null);

        this.m_resumeService.updateResume(tag, this.editSkills())
                            .pipe(takeUntilDestroyed(this.m_destroyRef))
                            .subscribe({ next: (updated: Resume) => { this.resumes.update((list) => list.map((r) => (r.tag === tag ? updated : r)));
                                                                      this.editSkillsSaving.set(false);
                                                                      this.editingResumeTag.set(null); },
                                         error: () => { this.editSkillsError.set('Could not update skills.');
                                                        this.editSkillsSaving.set(false); }});
    }

    public startEditSkills(resume: Resume): void {
        this.editingResumeTag.set(resume.tag);
        this.editSkills.set([...resume.skills]);
        this.editSkillsError.set(null);
    }

    public removeEditSkill(skill: string): void {
        this.editSkills.update((list) => list.filter((s) => s !== skill));
    }

    public updateTag(value: string): void {
        this.tag.set(value);
    }

    private loadFile(file: File): void {
        this.selectedFile.set(file);
        this.extractedText.set(null);
        this.extractError.set(null);
        this.saveSuccess.set(false);
        this.extracting.set(true);

        this.m_resumeService.extractText(file).then((text) => { this.extractedText.set(text);
                                                                this.extracting.set(false);})
                                              .catch((error: unknown) => { this.extractError.set(error instanceof Error ? error.message : 'Could not read that file.');
                                                                           this.extracting.set(false); });
    }

    private resetForm(): void {
        this.selectedFile.set(null);
        this.extractedText.set(null);
        this.tag.set('');
        this.skills.set([]);
    }
}